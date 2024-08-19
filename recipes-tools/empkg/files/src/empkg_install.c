/*
 * empkg_install.c - App install/uninstall functions
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
#include "empkg_json.h"
#include "empkg_lock.h"
#include "empkg_log.h"
#include "empkg_register.h"
#include "empkg_tar.h"
#include "empkg_users.h"

static int empkg_install(const char *path) {
	/* get app id -> manifest.json -> id
	 * is arch supported? -> manifest.json -> arch == /etc/opkg/arch.conf
	 * get manifest.json -> version, is_installed? is_new = false, get version -> is_new? "Installing version_new":"Updating version_old to version_new"
	 */
	struct empkg_new_t new;
	char *appmanifest;
	json_t *json;
	char *appdir, *installdir, *uninstalldir, *appdata;

	appmanifest = empkg_tar_pkg_info(path);

	json = json_loads(appmanifest, 0, NULL);
	free(appmanifest);

	if (json && json_is_object(json)) {
		json_t *property = json_object_get(json, "id");
		new.id = json_string_value(property);

		property = json_object_get(json, "version");
		new.version = json_string_value(property);

		property = json_object_get(json, "arch");
		new.arch = json_string_value(property);
	} else {
		log_message("empkg: Error in reading manifest.json of %s\n", path);
		return ERRORCODE;
	}

	if (!empkg_is_arch_supported(new.arch)) {
		log_message("empkg: Unable to install '%s': unsupported platform '%s'\n", path, new.arch);
		return ERRORCODE;
	}

	if (appdb_is(INSTALLED, new.id)) {
		char *version_old = appdb_get_version((char *)new.id);
		log_message("empkg: Updating app: %s from %s to %s...\n", new.id, version_old, new.version);
	} else {
		log_message("empkg: Installing app: %s %s...\n", new.id, new.version);
	}

	/* Fill our vars with paths */
	if ((asprintf(&appdir, "%s/installed/%s", gAPPDIR, new.id) == -1) ||
	    (asprintf(&installdir, "%s/tmp/install", gAPPDIR) == -1) ||
	    (asprintf(&uninstalldir, "%s/tmp/uninstall", gAPPDIR) == -1) ||
	    (asprintf(&appdata, "%s/data.tar.xz", installdir) == -1))
		return ERRORCODE;

	empkg_reset_appdir("tmp");
	empkg_fops_mkdir(installdir);
	empkg_tar_pkg_extract(path, installdir); /* extract empkg (data.tar.xz, manifest.json) to /apps/tmp/install */
	empkg_tar_pkg_extract(appdata, installdir); /* extract data.tar.xz (app binary etc.) */
	empkg_fops_rm(appdata); /* remove data.tar.xz */
	free(appdata);

	/* App replacement, wrapped in syncs for atomic action */
	sync();
	if (appdb_is(INSTALLED, new.id))
		empkg_fops_mv(appdir, uninstalldir); /* move current app in appdir to apptmp for removal */
	empkg_fops_mv(installdir, appdir);
	sync();
	/* Get rid of old app */
	empkg_reset_appdir("tmp");

	free(appdir);
	free(installdir);
	free(uninstalldir);

	appdb_scan_installed();

	if (config.enable) {
		if (!appdb_is(DISABLED, new.id))
			empkg_enable(new.id);
	} else {
		empkg_dbus(new.id);
	}

	return 0;
}

/*
 *	get_lock
 *	pkg_install "$1"
 *	update_www
 *	update_licenses
 */
int app_install(const char *empkg) {
	int ret;

	if (!empkg_lock()) {
		log_message("empkg: Could not get lock.\n");
		return ERRORCODE;
	}

	ret = empkg_install(empkg);
	if (ret)
		return ret;

	ret = empkg_process_reload_request();
	if (ret)
		return ret;

	ret = empkg_update_www();
	if (ret)
		return ret;

	empkg_update_licenses();

	empkg_unlock();

	return 0;
};

static int empkg_uninstall(const char *id) {
	const char *appdir = appdb_get_path(P_INSTALLED, id);
	char *uninstalldir;

	log_message("empkg: Uninstalling %s...\n", id);

	if (!appdb_is(INSTALLED, id) && !appdb_is(ENABLED, id)) {
		log_message("empkg: app '%s' not found.\n", id);
		return ERRORCODE;
	}

	/* empkg_disable() prints that it will not disable essential apps. */
	empkg_disable(id);

	/* Fill our vars with paths */
	if (asprintf(&uninstalldir, "%s/tmp/uninstall", gAPPDIR) == -1)
		return ERRORCODE;

	empkg_reset_appdir("tmp");
	empkg_fops_mv(appdir, uninstalldir); /* move current app in appdir to apptmp for removal */
	appdb_set(INSTALLED, id, 0);

	free(uninstalldir);

	if (appdb_is(BUILTIN, id)) {
		log_message("empkg: Note: '%s' is builtin, only uninstalling updates.\n", id);
		app_register(id);
	}

	empkg_users_sync_app_users_and_dirs(id);
	empkg_dbus(id);
	sync();
	empkg_reset_appdir("tmp");

	return 0;
};

int app_uninstall(const char *id) {
	int ret;

	if (!empkg_lock()) {
		log_message("empkg: Could not get lock.\n");
		return ERRORCODE;
	}

	ret = empkg_uninstall(id);
	if (ret)
		return ret;

	ret = empkg_update_www();
	if (ret)
		return ret;

	empkg_update_licenses();

	empkg_unlock();

	return 0;
}
