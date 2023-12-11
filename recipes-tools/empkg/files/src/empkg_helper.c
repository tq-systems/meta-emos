/*
 * empkg_helper.c - Various helper functions
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg.h"
#include "empkg_fops.h"

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
	char *configdir = gCONFIGDIR;
	char *rundirapps = gRUNDIRAPPS;
	char *userdbrundir = gUSERDBRUNDIR;
	char *userdbstoredir = gUSERDBSTOREDIR;

	empkg_fops_mkdir(configdir);
	empkg_fops_mkdir(rundirapps);
	empkg_fops_mkdir(userdbrundir);
	empkg_fops_mkdir(userdbstoredir);

	if (mount(userdbstoredir, userdbrundir, NULL, MS_BIND, NULL))
		fprintf(stderr, "Error mounting %s to %s (%s)\n", userdbstoredir, userdbrundir, strerror(errno));
}
