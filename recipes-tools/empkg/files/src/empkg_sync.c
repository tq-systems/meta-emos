/*
 * empkg_sync.c - App sync functions
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg.h"
#include "empkg_appdb.h"
#include "empkg_dbus.h"
#include "empkg_enable.h"
#include "empkg_fops.h"
#include "empkg_helper.h"
#include "empkg_lock.h"
#include "empkg_log.h"
#include "empkg_register.h"
#include "empkg_users.h"

static int app_sync_cb_eol(const char *id) {
	int ret = 0;

	if (!appdb_is(ESSENTIAL, id))
		ret = empkg_disable(id);

	return ret;
}

/*
 * Registers all builtin apps into the main app dir, removes
 * symlinks for apps that have disappered. New apps are automatically
 * enabled.
 *
 * This is an internal command (called from emos-init), so it does not
 * appear in usage
 */
int app_sync(void) {
	char *builtindir, *installeddir, *link, *confpath;
	DIR *dircheck;
	struct dirent **ent;
	int i = 0, n;
	int defer_count = appdb_get_n_apps();
	char *new_apps[64];
	char **new = new_apps, **enable_new = new_apps;

	if (!empkg_lock()) {
		log_message("empkg: Could not get lock.\n");
		return ERRORCODE;
	}

	empkg_init_dirs();

	/* Check each link in /apps/enabled/$app if $app exists in /apps/installed (= is installed)
	 * and if not empkg_disable() it, to remove its link in /apps/enabled
	 */
	n = scandir(gENABLEDDIR, &ent, scandir_filter_valid_id_link, alphasort);

	while (i < n) {
		/* Disable apps that are not installed anymore */
		if (!appdb_is(INSTALLED, ent[i]->d_name))
			empkg_disable(ent[i]->d_name);
		free(ent[i]);
		i++;
	}
	free(ent);

	/* Check each link in /apps/installed/$app if it exists in /opt/apps/$app (= is builtin) */
	n = scandir(gINSTALLEDDIR, &ent, scandir_filter_valid_id_link, alphasort);
	i = 0;

	while (i < n) {
		/* Remove installed symlinks for apps that are not builtin anymore */
		if ((asprintf(&builtindir, "%s/%s", gBUILTINDIR, ent[i]->d_name) == -1) ||
		    (asprintf(&link, "%s/%s", gINSTALLEDDIR, ent[i]->d_name) == -1))
			return ERRORCODE;
		dircheck = opendir(builtindir);
		free(builtindir);
		if (!dircheck)
			empkg_fops_rm(link);
		free(link);
		closedir(dircheck);
		free(ent[i]);
		i++;
	}
	free(ent);

	empkg_fops_mkdir(gINSTALLEDDIR);
	empkg_fops_mkdir(gDBUSDIR);

	n = scandir(gBUILTINDIR, &ent, scandir_filter_valid_id, alphasort);
	i = 0;

	/* Link builtin apps into $APPDIR/installed */
	while (i < n) {
		if (ent[i]->d_type == DT_DIR) {
			struct stat sb;
			int err;

			if (asprintf(&installeddir, "%s/%s", gINSTALLEDDIR, ent[i]->d_name) == -1)
				return ERRORCODE;

			/* if [ -e "$APPDIR/installed/$app" ] && ! is_essential "$app"; then */
			err = lstat(installeddir, &sb);
			free(installeddir);
			if (!err && (S_ISLNK(sb.st_mode) || S_ISDIR(sb.st_mode)) &&
			    !appdb_is(ESSENTIAL, ent[i]->d_name)) {
				i++;
				continue;
			}

			app_register(ent[i]->d_name);

			*new++ = strdup(ent[i]->d_name);
		}
		free(ent[i]);
		i++;
	}
	free(ent);

	/* # Remove obsolete D-Bus policy files */
	n = scandir(gDBUSDIR, &ent, scandir_filter_default, alphasort);
	i = 0;

	while (i < n) {
		if (!appdb_is(ENABLED, remove_suffix(ent[i]->d_name))) {
			if (asprintf(&confpath, "%s/%s", gDBUSDIR, ent[i]->d_name) == -1)
				return ERRORCODE;
			empkg_fops_rm(confpath);
			free(confpath);
		}
		free(ent[i]);
		i++;
	}
	free(ent);

	sync();
	appdb_all(ENABLED, empkg_users_sync_app_users_and_dirs);
	while (appdb_count_deferred() && --defer_count)
		appdb_all(DEFERRED, empkg_users_sync_app_users_and_dirs);
	sync();
	if (!defer_count)
		log_message("empkg: Warning: Maximum deferral reached.");

	appdb_all(ENABLED, empkg_dbus);

	/* Delay enabling apps until all new apps have been registered,
	 * so potential dependencies between new apps are satisfied
	 */
	while (enable_new < new) {
		if (!appdb_is(DISABLED, *enable_new))
			empkg_enable(*enable_new);
		enable_new++;
	}

	/* EOL: disable non-essential apps */
	if (config.eol)
		appdb_all(ENABLED, app_sync_cb_eol);

	empkg_process_reload_request();

	empkg_update_www();

	empkg_update_licenses();

	empkg_unlock();

	return 0;
};
