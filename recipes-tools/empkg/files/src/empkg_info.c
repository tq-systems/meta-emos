/*
 * empkg_info.c - Info about empkg file
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg.h"
#include "empkg_json.h"
#include "empkg_log.h"
#include "empkg_tar.h"

int info(const char *path) {
	char *appmanifest;
	char *output;

	appmanifest = empkg_tar_pkg_info(path);
	if (!appmanifest) {
		log_message("empkg: Error reading app_info\n");
		return ERRORCODE;
	}

	output = empkg_json_pretty(appmanifest);
	fprintf(stderr, "%s\n", output);
	free(appmanifest);
	free(output);

	return 0;
};
