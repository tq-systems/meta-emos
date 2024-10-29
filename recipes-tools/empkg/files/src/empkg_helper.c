/*
 * empkg_helper.c - Various helper functions
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg.h"
#include "empkg_appdb.h"
#include "empkg_fops.h"
#include "empkg_log.h"

struct sd_reload_command {
	char *cmd;
	char *app;
	struct sd_reload_command *next;
};
struct sd_reload_command sd_reload_list;

void empkg_request_daemon_reload(char *cmd, char *app) {
	struct sd_reload_command *new, *tail = &sd_reload_list;

	sd_reload_list.cmd = "daemon-reload";

	if (cmd && app) {
		/* find last entry with next == NULL
		 * malloc new sd_reload_command struct, assign cmd/app
		 * assign to next pointer
		*/
		new = calloc(1, sizeof(struct sd_reload_command));
		if (new) {
			new->cmd = strdup(cmd);
			new->app = strdup(app);

			while (tail->next)
				tail = tail->next;
			tail->next = new;
		}
	}
}

int empkg_process_reload_request(void) {
	struct sd_reload_command *entry = &sd_reload_list;
	struct sd_reload_command *next;
	char *syscall;
	int ret = 0;

	while (entry && entry->cmd) {
		if (asprintf(&syscall, "systemctl %s %s", entry->cmd, entry->app?entry->app:"") == -1)
			return ERRORCODE;

		if (!strcmp(entry->cmd, "daemon-reload"))
			log_message("empkg: reloading system daemons, this could take a bit...\n");

		ret = system(syscall);
		if (ret)
			log_message("empkg: error executing %s\n", syscall);
		free(syscall);

		/* additional entries malloc'd? */
		if (entry == &sd_reload_list) {
			entry = entry->next;
		} else {
			/* store next one after */
			next = entry->next;
			/* free current malloc'd entry */
			free(entry);
			/* go on to the next */
			entry = next;
		}
	}

	return ret;
}

#define WWWDIRTMP "/tmp/tmp.www"
static int empkg_update_www_cb(const char *id) {
	const char *index = appdb_get_path(P_INDEX, id);
	const char *lang = appdb_get_path(P_LANG, id);
	const char *wwwdir = appdb_get_path(P_WWW, id);
	int err;
	struct stat sbi, sbl;
	char *appstmp = WWWDIRTMP"/apps.plain";
	char *langstmp = WWWDIRTMP"/langs.plain";
	FILE *fappstmp, *flangstmp;
	char *link;
	int digest_length = EVP_MD_size(EVP_md5());

	err = lstat(index, &sbi);
	err |= lstat(lang, &sbl);

	/* [ -r "$index ] || continue */
	if (err || !S_ISREG(sbi.st_mode) || !S_ISREG(sbl.st_mode))
		return 0;

	fappstmp = fopen(appstmp, "a");
	flangstmp = fopen(langstmp, "a");

	if (!fappstmp || !flangstmp)
		return ERRORCODE;

	/* get index.js and lang.js md5sum for this app */
	appdb_check(MD5SUMS, id);

	fprintf(fappstmp, "%s/index.js?", id);
	fprintf(flangstmp, "%s/i18n/lang.js?", id);
	for (int i = 0; i < digest_length; i++) {
		fprintf(fappstmp, "%02x", appdb_get_md5sums(id)[INDEX][i]);
		fprintf(flangstmp, "%02x", appdb_get_md5sums(id)[LANG][i]);
	}
	fprintf(fappstmp, "\n");
	fprintf(flangstmp, "\n");

	fclose(fappstmp);
	fclose(flangstmp);

	/* create symlink to app's wwwdir in WWWDIRTMP */
	if (asprintf(&link, "%s/%s", WWWDIRTMP, id) == -1)
		return ERRORCODE;

	empkg_fops_symlink(wwwdir, link);
	free(link);

	return 0;
}

int empkg_update_www(void) {
	char *appstmp = WWWDIRTMP"/apps.plain";
	char *langstmp = WWWDIRTMP"/langs.plain";
	FILE *fappstmp, *flangstmp;
	FILE *fapps, *flangs;
	char *wwwdir, *apps, *langs;
	char buffer[256];
	json_t *root = json_array();
	int ret;

	empkg_fops_mkdir(WWWDIRTMP);
	ret = appdb_all(ENABLED, empkg_update_www_cb);
	if (ret)
		log_message("empkg: Error update_www()\n");

	/* convert md5sums in WWWTMPDIR/ *.plain to temporary WWWTMPDIR/ *.json
	 * move from WWWTMP to wwwdir
	 */
	if ((asprintf(&wwwdir, "%s/www", gAPPDIR) == -1) ||
	    (asprintf(&apps, "%s/apps.json", WWWDIRTMP) == -1) ||
	    (asprintf(&langs, "%s/langs.json", WWWDIRTMP) == -1))
		return ERRORCODE;

	fappstmp = fopen(appstmp, "r");
	flangstmp = fopen(langstmp, "r");
	fapps = fopen(apps, "w");
	flangs = fopen(langs, "w");
	if (!fappstmp || !flangstmp || !fapps || !flangs)
		return ERRORCODE;

	while (!ret && fgets(buffer, sizeof(buffer), fappstmp)) {
		buffer[strlen(buffer) - 1] = '\0';
		ret = json_array_append(root, json_string(buffer));
	}
	json_dumpf(root, fapps, JSON_COMPACT);
	fclose(fappstmp);
	fclose(fapps);
	empkg_fops_rm(appstmp);
	json_array_clear(root);

	while (fgets(buffer, sizeof(buffer), flangstmp)) {
		buffer[strlen(buffer) - 1] = '\0';
		ret = json_array_append(root, json_string(buffer));
	}
	json_dumpf(root, flangs, JSON_COMPACT);
	fclose(flangstmp);
	fclose(flangs);
	empkg_fops_rm(langstmp);
	json_array_clear(root);

	/* and for moving to wwwdir do: sync; rm -rf wwwdir; mv wwwtmpdir wwwdir; sync */
	sync();
	empkg_fops_rm(wwwdir);
	empkg_fops_mv(WWWDIRTMP, wwwdir);
	sync();

	free(wwwdir);
	free(apps);
	free(langs);
	json_decref(root);

	return ret;
}

#define LICDIRTMP "/tmp/tmp-lic"
static int empkg_update_licenses_callback(const char *id) {
	char *applicdir, *link;
	DIR *dir;

	if ((asprintf(&applicdir, "%s/%s/license", gINSTALLEDDIR, id) == -1) ||
	    (asprintf(&link, "%s/%s", LICDIRTMP, id) == -1))
		return ERRORCODE;

	dir = opendir(applicdir);
	if (!dir) {
		free(applicdir);
		free(link);
		return 0;
	}
	closedir(dir);

	empkg_fops_symlink(applicdir, link);

	free(applicdir);
	free(link);

	return 0;
}

void empkg_update_licenses(void) {
	const char *licdir = "/apps/licenses";

	empkg_fops_mkdir(LICDIRTMP);
	appdb_all(INSTALLED, empkg_update_licenses_callback);

	sync();
	empkg_fops_rm(licdir);
	empkg_fops_mv(LICDIRTMP, licdir);
	sync();
}

/* why does empkg(bash) compare against every line in /etc/opkg/arch.conf ? */
int empkg_is_arch_supported(const char *arch) {
	struct utsname uts;

	/* armv5: uts.machine == armv5tejl */
	uname(&uts);
	if (!strcmp("all", arch))
		return 1;
	else if (!strcmp("armv5e", arch) && !strcmp("armv5tejl", uts.machine))
		return 1;
	else if (!strcmp("aarch64", arch) && !strcmp("aarch64", uts.machine))
		return 1;
	else
		return 0;
}

void empkg_reset_appdir(const char *sub) {
	char *path = empkg_fops_abspath(gAPPDIR, sub);

	empkg_fops_rm(path);
	empkg_fops_mkdir(path);

	free(path);
}

char *remove_suffix(char *filename) {
	char *suffix = strrchr(filename, '.');
	if (suffix)
		*suffix = '\0';

	return filename;
}

void empkg_init_dirs(void) {
	const char * const init_dirs[] = {
		gCONFIGDIR,
		gRUNDIRAPPS,
		gUSERDBRUNDIR,
		gUSERDBSTOREDIR
		gCONFIGSYSTEMDIR,
		gCONFIGAUTHDIR,
		gDATAAPPSDIR
	};

	for (unsigned int i = 0; i < ARRAY_SIZE(init_dirs); i++)
		empkg_fops_mkdir(init_dirs[i]);

	if (mount(gUSERDBSTOREDIR, gUSERDBRUNDIR, NULL, MS_BIND, NULL))
		log_message("empkg: Error mounting %s to %s (%s)\n", gUSERDBSTOREDIR, gUSERDBRUNDIR, strerror(errno));
}
