/*
 * empkg_appdb.c - Handles setup/requests of/to app properties
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg_appdb.h"
#include "empkg_fops.h"
#include "empkg_json.h"
#include "empkg_log.h"

struct appdb_t *appdb = NULL;
int appdb_napps;

#define APPDB_INIT_VAL (0xFF)

const char pattern[NPATHS][APPDB_MAX_PATH] = {
	[P_BUILTIN]   = gBUILTINDIR"/%s",
	[P_CONFIGDIR] = gCONFIGDIR"/%s",
	[P_DATADIR]   = "/data/apps/%s",
	[P_DBUSAPP]   = gINSTALLEDDIR"/%s/dbus.conf",
	[P_DBUSCONF]  = gAPPDIR"/dbus/%s.conf",
	[P_ENABLED]   = gENABLEDDIR"/%s",
	[P_FWCONF]    = gINSTALLEDDIR"/%s/em-firewall.conf",
	[P_INDEX]     = gINSTALLEDDIR"/%s/www/index.js",
	[P_INSTALLED] = gINSTALLEDDIR"/%s",
	[P_LANG]      = gINSTALLEDDIR"/%s/www/i18n/lang.js",
	[P_MANIFEST]  = gINSTALLEDDIR"/%s/manifest.json",
	[P_RUNDIR]    = gRUNDIRAPPS"/%s",
	[P_SERVICE]   = gINSTALLEDDIR"/%s/em-app-%s.service",
	[P_WWW]       = gINSTALLEDDIR"/%s/www"
};

int scandir_filter_default(const struct dirent *ent) {
	if (ent->d_name[0] == '.')
		return 0;

	return 1;
}

int scandir_filter_valid_id(const struct dirent *ent) {
	if (!scandir_filter_default(ent))
		return 0;

	if (!appdb_is_valid_id(ent->d_name)) {
		log_message("empkg: %s:%d invalid id: %s\n", __func__, __LINE__, ent->d_name);
		return 0;
	}

	return 1;
}

int scandir_filter_valid_id_link(const struct dirent *ent) {
	if (!scandir_filter_valid_id(ent))
		return 0;

	if (ent->d_type != DT_LNK)
		return 0;

	return 1;
}

static void appdb_init(struct appdb_t *entry) {
	entry->id = NULL;
	entry->name = NULL;
	entry->version = NULL;
	entry->valid_name = APPDB_INIT_VAL;
	entry->autostart = APPDB_INIT_VAL;
	entry->minsysmem = APPDB_INIT_VAL;
	entry->builtin = APPDB_INIT_VAL;
	entry->disabled = APPDB_INIT_VAL;
	entry->enabled = APPDB_INIT_VAL;
	entry->essential = APPDB_INIT_VAL;
	entry->fwconf = APPDB_INIT_VAL;
	entry->systemd = APPDB_INIT_VAL;
	entry->installed = APPDB_INIT_VAL;
	entry->deferred = 0;
	entry->next = NULL;
	memset(entry->paths, 0, NPATHS * APPDB_MAX_PATH);
	memset(entry->md5sums, APPDB_INIT_VAL, NSUMS*sizeof(md5sums_t));
}

static int appdb_add(const char *id) {
	struct appdb_t *new, *tail;
	int i;

	new = malloc(sizeof(struct appdb_t));
	if (!new)
		return ERRORCODE;

	appdb_init(new);
	new->id = strdup(id);

	/* Fill in pre-defined paths */
	for(i = 0; i < NPATHS; i++) {
		if (i == P_SERVICE)
			snprintf(new->paths[i], strlen(pattern[i]) + 2*strlen(id) + 1, pattern[i], id, id);
		else
			snprintf(new->paths[i], strlen(pattern[i]) + strlen(id) + 1, pattern[i], id);
	}

	/* Corner case: updater-servicecloudclient requires socket in /run/em/apps/updater!
	 * Apply custom P_RUNDIR for that app-id
	 */
	if (!strcmp("updater-servicecloudclient", id)) {
		memset(new->paths[P_RUNDIR], 0, APPDB_MAX_PATH);
		snprintf(new->paths[P_RUNDIR], strlen("/run/em/apps/updater") + 1, "/run/em/apps/updater");
	}

	if (!appdb) {
		/* initialise appdb pointer to this new element */
		appdb = new;
	} else {
		/* find tail, add new entry after tail */
		tail = appdb;
		while (tail->next)
			tail = tail->next;
		tail->next = new;
	}
	appdb_napps++;

	return 0;
}

static struct appdb_t *appdb_find_entry(const char *id) {
	struct appdb_t *entry = appdb;

	/* start with first entry (appdb).
	 * Check for matching id.
	 * Continue with ->next entry;
	 */
	while (entry) {
		if (!strcmp(id, entry->id))
			return entry;

		entry = entry->next;
	}

	return NULL;
}

char *appdb_get_path(const int path, const char *id) {
	struct appdb_t *a = appdb_find_entry(id);

	if (!a)
		return NULL;

	return a->paths[path];
}

/* appdb_check_autostart(app) ->  is_enabled && app_info "$app" | jq -e '.autostart != false' -> set_autostart(app, value) */
static void appdb_check_autostart(const char *id) {
	int autostart;

	if (appdb_is(ENABLED, id) && appdb_is(INSTALLED, id)) {
		autostart = empkg_json_get_int(id, "autostart");

		/* default true, if not found */
		if (autostart < 0)
			autostart = 1;
	} else { /* not enabled or not installed */
		autostart = 0;
	}

	appdb_set(AUTOSTART, id, autostart);

	return;
}

/* appdb_check_builtin -> [ -d "$BUILTIN_DIR/$1" ] -> set_builtin(0/1) */
static void appdb_check_builtin(const char *id) {
	const char *path = appdb_get_path(P_BUILTIN, id);
	struct stat sb;
	int err;
	int val = 0;

	err = lstat(path, &sb);
	/* allow /opt/apps/id to be a symbolic link, check link target then */
	if (!err && S_ISLNK(sb.st_mode))
		err = stat(path, &sb);
	if (!err && S_ISDIR(sb.st_mode))
		val = 1;

	appdb_set(BUILTIN, id, val);
	return;
}

/* appdb_check_disabled -> is_installed "$app" && app_info "$app" | jq -e '.disabled == true' -> set_disabled(0/1) */
/* Property not found == false */
static void appdb_check_disabled(const char *id) {
	int disabled = 0;

	if (appdb_is(INSTALLED, id))
		disabled = empkg_json_get_int(id, "disabled");

	if (disabled < 0)
		disabled = 0;

	appdb_set(DISABLED, id, disabled);

	return;
}

/* appdb_check_enabled(app) -> [ -L "$APPDIR/enabled/$1" ] + set_enabled(0/1) */
static void appdb_check_enabled(const char *id) {
	const char *path = appdb_get_path(P_ENABLED, id);
	struct stat sb;
	int err;
	int enabled = 0;

	err = lstat(path, &sb);
	if (!err && S_ISLNK(sb.st_mode))
		enabled = 1;

	appdb_set(ENABLED, id, enabled);

	return;
}

/* appdb_check_essential -> is_builtin "$app" && app_info "$app" | jq -e '.essential' -> set_essential(0/1) */
/* Property not found = false */
static void appdb_check_essential(const char *id) {
	int essential = 0;

	if (appdb_is(BUILTIN, id))
		essential = empkg_json_get_int(id, "essential");

	if (essential < 0)
		essential = 0;

	appdb_set(ESSENTIAL, id, essential);

	return;
}

/* appdb_check_fwconf -> does the app have a firewall config
 * expects either em-firewall.conf or em-fw.conf (legacy)
 */
static void appdb_check_fwconf(const char *id) {
	char *path = appdb_get_path(P_FWCONF, id);
	struct stat sb;
	int err;
	int fwconf = 0;

	err = lstat(path, &sb);
	if (!err && S_ISREG(sb.st_mode))
		fwconf = 1;

	/* em-firewall.conf not found or not a file? try em-fw.conf */
	if (!fwconf) {
		const char *pattern = gINSTALLEDDIR"/%s/em-fw.conf";
		snprintf(path, strlen(pattern) + strlen(id) + 1, pattern, id);
		err = lstat(path, &sb);
		if (!err && S_ISREG(sb.st_mode))
			fwconf = 1;
	}

	appdb_set(FWCONF, id, fwconf);
	return;
}

/* appdb_check_installed -> [ -e "$APPDIR/installed/$1" ] -> set_installed(0/1) */
static void appdb_check_installed(const char *id) {
	const char *path = appdb_get_path(P_INSTALLED, id);
	struct stat sb;
	int err;
	int val = 0;

	err = lstat(path, &sb);
	if (!err && (S_ISDIR(sb.st_mode) || S_ISLNK(sb.st_mode)))
		val = 1;

	appdb_set(INSTALLED, id, val);
	return;
}

/* check if minimal system memory requirement is met.
 * The defaule value is inverted here.
 * return 1 if totalram is more than required OR if no minimum defined!
 * return 0 if not enough memory
 */
#define MANIFEST_MINSYSMEM_KEY "min_sys_mem"
static void appdb_check_minsysmem(const char *id) {
	int minsysmem = 1; /* default */
	struct sysinfo info;
	const char *manifest_minsysmem_val = empkg_json_get_char(id, MANIFEST_MINSYSMEM_KEY);
	uint64_t minsysmem_bytes;

	if (manifest_minsysmem_val && strlen(manifest_minsysmem_val) > 0) {
		minsysmem_bytes = strtoull(manifest_minsysmem_val, NULL, 10);

		/* conversion error or no minimum defined, return default 1 */
		if (!errno) {
			/* Query certain statistics on memory usage etc. */
			sysinfo(&info);

			/* if physical memory is less than required memory (10% margin), minimum is not met */
			if (info.totalram < 0.90 * minsysmem_bytes)
				minsysmem = 0;
		}
	}

	appdb_set(MINSYSMEM, id, minsysmem);

	return;
}

/* add new apps in installed-dir to appdb */
int appdb_scan_installed(void) {
	struct dirent **ent;
	int i = 0, n;
	char *path = gINSTALLEDDIR;

	empkg_fops_mkdir(path);

	n = scandir(path, &ent, scandir_filter_valid_id, alphasort);

	if (n == -1) {
		log_message("empkg: opendir error");
		return ERRORCODE;
	}

	while (i < n) {
		if (!appdb_find_entry(ent[i]->d_name))
			appdb_add(ent[i]->d_name);
		appdb_set(INSTALLED, ent[i]->d_name, 1);
		free(ent[i]);
		i++;
	}
	free(ent);

	return 0;
}

/* add new apps in builtin-dir to appdb */
int appdb_scan_builtin(void) {
	struct dirent **ent;
	int i = 0, n;

	n = scandir(gBUILTINDIR, &ent, scandir_filter_valid_id, alphasort);

	if (n == -1) {
		log_message("empkg: opendir error");
		return ERRORCODE;
	}

	while (i < n) {
		if (!appdb_find_entry(ent[i]->d_name))
				appdb_add(ent[i]->d_name);
		appdb_set(BUILTIN, ent[i]->d_name, 1);
		free(ent[i]);
		i++;
	}
	free(ent);

	return 0;
}

/* add new apps in enabled-dir to appdb */
int appdb_scan_enabled(void) {
	struct dirent **ent;
	int i = 0, n;
	const char *path = gENABLEDDIR;

	empkg_fops_mkdir(path);

	n = scandir(path, &ent, scandir_filter_valid_id_link, alphasort);

	if (n == -1) {
		log_message("empkg: opendir error");
		return ERRORCODE;
	}

	while (i < n) {
		if (!appdb_find_entry(ent[i]->d_name))
			appdb_add(ent[i]->d_name);
		appdb_set(ENABLED, ent[i]->d_name, 1);
		free(ent[i]);
		i++;
	}
	free(ent);

	return 0;
}

/* appdb_check_systemd -> is_installed "$1" && [ -e "$APPDIR/installed/$1/em-app-$1.service" ] -> set_systemd(0/1) */
static void appdb_check_systemd(const char *id) {
	const char *path = appdb_get_path(P_SERVICE, id);
	struct stat sb;
	int err;
	int systemd = 0;

	err = lstat(path, &sb);
	if (!err && S_ISREG(sb.st_mode))
		systemd = 1;

	appdb_set(SYSTEMD, id, systemd);
	return;
}

static void appdb_check_version(const char *id) {
	if (appdb_get(INSTALLED, id)) {
		const char *version = empkg_json_get_char(id, "version");

		if (!version)
			version = "(not found)";

		appdb_set_version(id, version);
	}

	return;
}

static void appdb_check_md5sums(const char *id) {
	struct appdb_t *a = appdb_find_entry(id);
	const char *index = appdb_get_path(P_INDEX, id);
	const char *lang = appdb_get_path(P_LANG, id);
	FILE *findex, *flang;
	unsigned char buffer[256];
	size_t bytes;
	EVP_MD_CTX *ctx;
	unsigned char *digest;
	unsigned int diglen = EVP_MD_size(EVP_md5());

	if (!a) {
		log_message("empkg: app '%s' not found\n", id);
		return;
	}

	findex = fopen(index, "r");
	flang = fopen(lang, "r");

	if (!findex || !flang) {
		fclose(findex);
		fclose(flang);
		return;
	}

	/* index.js */
	/* MD5_Init */
	ctx = EVP_MD_CTX_new();
	EVP_DigestInit_ex(ctx, EVP_md5(), NULL);
	while (0 < (bytes = fread(buffer, 1, sizeof(buffer), findex)))
		EVP_DigestUpdate(ctx, buffer, bytes); /* MD5_Update */
	/* MD5_Final */
	digest = OPENSSL_malloc(diglen);
	EVP_DigestFinal_ex(ctx, digest, &diglen);
	EVP_MD_CTX_free(ctx);
	memcpy(a->md5sums[INDEX], digest, diglen);
	fclose(findex);

	/* lang.js */
	ctx = EVP_MD_CTX_new();
	EVP_DigestInit_ex(ctx, EVP_md5(), NULL);
	while (0 < (bytes = fread(buffer, 1, sizeof(buffer), flang)))
		EVP_DigestUpdate(ctx, buffer, bytes);
	digest = OPENSSL_malloc(diglen);
	EVP_DigestFinal_ex(ctx, digest, &diglen);
	EVP_MD_CTX_free(ctx);
	memcpy(a->md5sums[LANG], digest, diglen);
	fclose(flang);

	return;
}

void appdb_check(const dbp prop, const char *id) {
	switch (prop) {
	case AUTOSTART:
		appdb_check_autostart(id);
		break;
	case BUILTIN:
		appdb_check_builtin(id);
		break;
	case DISABLED:
		appdb_check_disabled(id);
		break;
	case ENABLED:
		appdb_check_enabled(id);
		break;
	case ESSENTIAL:
		appdb_check_essential(id);
		break;
	case FWCONF:
		appdb_check_fwconf(id);
		break;
	case INSTALLED:
		appdb_check_installed(id);
		break;
	case MINSYSMEM:
		appdb_check_minsysmem(id);
		break;
	case SYSTEMD:
		appdb_check_systemd(id);
		break;
	case MD5SUMS:
		appdb_check_md5sums(id);
		break;
	case VERSION:
		appdb_check_version(id);
		break;
	case DEFERRED:
		break;
	};
};

/* appdb_get_... - starts check for property and returns property of given app */
int appdb_get(const dbp prop, const char *id) {
	struct appdb_t *a = appdb_find_entry(id);
	int ret = 0;

	if (!a)
		return -1;

	switch (prop) {
	case AUTOSTART:
		ret = a->autostart;
		break;
	case BUILTIN:
		ret = a->builtin;
		break;
	case DISABLED:
		ret = a->disabled;
		break;
	case ENABLED:
		ret = a->enabled;
		break;
	case ESSENTIAL:
		ret = a->essential;
		break;
	case FWCONF:
		ret = a->fwconf;
		break;
	case INSTALLED:
		ret = a->installed;
		break;
	case MINSYSMEM:
		ret = a->minsysmem;
		break;
	case DEFERRED:
		ret = a->deferred;
		break;
	case SYSTEMD:
		ret = a->systemd;
		break;
	case MD5SUMS:
		log_message("empkg: %s cannot handle MD5SUMS type. Use appdb_get_md5sums() instead.\n", __func__);
		ret = -1;
		break;
	case VERSION:
		log_message("empkg: %s cannot handle VERSION type. Use appdb_get_version() instead.\n", __func__);
		ret = -1;
		break;
	}

	if (ret == APPDB_INIT_VAL) {
		appdb_check(prop, id);
		ret = appdb_get(prop, id);
		/* possible infinite loop? */
	}

	return ret;
}

md5sums_t *appdb_get_md5sums(const char *id) {
	struct appdb_t *a = appdb_find_entry(id);
	unsigned int diglen = EVP_MD_size(EVP_md5());
	bool check_required = true;

	if (!a)
		return NULL;

	for(unsigned int i = 0; i < diglen; i++)
		if (a->md5sums[i][INDEX] != APPDB_INIT_VAL) {
			check_required = false;
			break;
		}

	if (check_required)
		appdb_check_md5sums(id);

	return a->md5sums;
}

char *appdb_get_version(const char *id) {
	struct appdb_t *a = appdb_find_entry(id);

	if (!a)
		return NULL;

	if (a->version == NULL)
		appdb_check_version(id);

	return a->version;
}

/* appdb_set_... - sets property of app to given integer */
int appdb_set(const dbp prop, const char *id, const int value) {
	struct appdb_t *a = appdb_find_entry(id);

	if (!a)
		return ERRORCODE;

	switch (prop) {
	case AUTOSTART:
		a->autostart = value;
		break;
	case BUILTIN:
		a->builtin = value;
		break;
	case DEFERRED:
		a->deferred = value;
		break;
	case DISABLED:
		a->disabled = value;
		break;
	case ENABLED:
		a->enabled = value;
		break;
	case ESSENTIAL:
		a->essential = value;
		break;
	case FWCONF:
		a->fwconf = value;
		break;
	case INSTALLED:
		a->installed = value;
		break;
	case MINSYSMEM:
		a->minsysmem = value;
		break;
	case SYSTEMD:
		a->systemd = value;
		break;
	case MD5SUMS:
		log_message("empkg: %s cannot handle MD5SUMS type. Use appdb_check_md5sums() instead.\n", __func__);
		return ERRORCODE;
		break;
	case VERSION:
		log_message("empkg: %s cannot handle VERSION type. Use appdb_set_version() instead.\n", __func__);
		return ERRORCODE;
		break;
	}

	return 0;
}

void appdb_set_version(const char *id, const char *value) {
	struct appdb_t *a = appdb_find_entry(id);

	if (a)
		a->version = strdup(value);
}

int appdb_get_n_apps()
{
	return appdb_napps;
}

int appdb_count_deferred()
{
	struct appdb_t *entry = appdb;
	int cnt = 0;

	while (entry) {
		if (entry->deferred)
			cnt++;

		entry = entry->next;
	}


	return cnt;
}

/* appdb_all_... - Tests property for all apps with appdb_get...
 *                 and calls callback function for each app.
 */
int appdb_all(const dbp prop, int (*callback_fn)(const char *)) {
	struct appdb_t *a = appdb;
	int ret = 0;

	if (!callback_fn) {
		log_message("empkg: ERROR: %s: callback_fn is NULL!\n", __func__);
		ret = ERRORCODE;
	}

	while (!ret && a) {
		if (a->id && appdb_get(prop, a->id) == 1)
			ret = callback_fn(a->id);
		a = a->next;
	}

	return ret;
}

bool appdb_is_valid_id(const char *id) {
	int ret = 1;

	while (*id) {
		if (!isdigit(*id) && !islower(*id) && *id != '-')
			ret = 0;
		id++;
	}

	return ret;
}
