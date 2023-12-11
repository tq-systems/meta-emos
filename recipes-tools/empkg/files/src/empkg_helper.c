/*
 * empkg_helper.c - Various helper functions
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg.h"
#include "empkg_fops.h"

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
			fprintf(stderr, "empkg: reloading system daemons, this could take a bit...\n");

		ret = system(syscall);
		if (ret)
			fprintf(stderr, "error executing %s\n", syscall);
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
