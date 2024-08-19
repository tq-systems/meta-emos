/*
 * empkg_register.c - App registering
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg.h"
#include "empkg_appdb.h"
#include "empkg_fops.h"
#include "empkg_log.h"

void app_register(const char *id) {
	const char *target = appdb_get_path(P_BUILTIN, id);
	const char *appdir = appdb_get_path(P_INSTALLED, id);

	struct stat sb;
	int err;

	if (!appdb_is(BUILTIN, id)) {
		log_message("empkg: tried to register app '%s', but it is not builtin.", id);
		return;
	}

	err = lstat(appdir, &sb);
	if (err && errno == ENOENT) {
		empkg_fops_symlink(target, appdir);
		/* appdb_scan_installed(); */
		appdb_set(INSTALLED, id, 1);
		appdb_check(SYSTEMD, id);
	}
}
