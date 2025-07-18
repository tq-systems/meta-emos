/*
 * empkg_enable.c - App enable/disable functions
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg.h"
#include "empkg_appdb.h"
#include "empkg_dbus.h"
#include "empkg_fops.h"
#include "empkg_helper.h"
#include "empkg_lock.h"
#include "empkg_log.h"
#include "empkg_users.h"

int empkg_enable(const char *id) {
	const char *link = appdb_get_path(P_ENABLED, id);
	char *installedtarget, *service;
	int ret;

	if (!appdb_is(INSTALLED, id)) {
		log_message("empkg: app '%s' not found.\n", id);
		return ERRORCODE;
	}

	if ((asprintf(&installedtarget, "../installed/%s", id) == -1) ||
	    (asprintf(&service, "em-app-%s.service", id) == -1))
		return ERRORCODE;

	ret = empkg_fops_mkdir(gENABLEDDIR);

	if (!ret)
		ret = empkg_fops_symlink(installedtarget, link);
	free(installedtarget);

	if (!ret) {
		appdb_set(ENABLED, id, 1);
		ret = empkg_users_sync_app_users_and_dirs(id);
	}

	if (!ret)
		ret = empkg_dbus(id);

	if (!ret && config.systemd && appdb_is(SYSTEMD, id)) {
		if (appdb_is(AUTOSTART, id))
			empkg_request_daemon_reload("restart", service);
		else
			empkg_request_daemon_reload("try-restart", service);
	}
	free(service);

	return ret;
};

int app_enable(const char *id) {
	int ret = 0;

	if (!empkg_lock()) {
		log_message("empkg: Could not get lock.\n");
		return ERRORCODE;
	}

	ret = empkg_enable(id);
	if (ret)
		return ret;

	ret = empkg_update_firewall_single(id);
	if (ret)
		return ret;

	ret = empkg_process_reload_request(NULL);
	if (ret)
		return ret;

	ret = empkg_update_www();
	if (ret)
		return ret;

	empkg_users_sync_app_users_and_dirs(id);

	empkg_unlock();

	return ret;
}

int empkg_disable(const char *id) {
	const char *enableddir = appdb_get_path(P_ENABLED, id);
	int ret;

	if (!appdb_is(INSTALLED, id) && !appdb_is(ENABLED, id)) {
		log_message("empkg: app '%s' not found.\n", id);
		return ERRORCODE;
	}

	if (appdb_is(ESSENTIAL, id)) {
		log_message("empkg: unable to disable essential app '%s'\n", id);
		return ERRORCODE;
	}

	ret = empkg_fops_rm(enableddir);

	appdb_set(ENABLED, id, 0);

	if (!ret)
		ret = empkg_dbus(id);

	return ret;
}

int app_disable(const char *id) {
	int ret;

	if (!empkg_lock()) {
		log_message("empkg: Could not get lock.\n");
		return ERRORCODE;
	}

	ret = empkg_disable(id);
	if (ret)
		return ret;

	ret = empkg_update_firewall_single(id);
	if (ret)
		return ret;

	if (config.systemd && appdb_is(SYSTEMD, id)) {
		char *service;

		if (asprintf(&service, "em-app-%s.service", id) == -1)
			return ERRORCODE;

		log_message("empkg: Stopping app: %s.\n", service);
		empkg_request_daemon_reload("stop", service);

		free(service);
	}

	/* update_firewall_single requests firewall restart */
	ret = empkg_process_reload_request(NULL);
	if (ret)
		return ret;

	ret = empkg_update_www();
	if (ret)
		return ret;

	empkg_unlock();

	return ret;
}
