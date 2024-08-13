/*
 * empkg_lock.c - File lock to prevent multiple instances
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg_lock.h"
#include "empkg_fops.h"

/* Path to lock for running only one instance */
static char *empkg_file_lock = "/var/lock/empkg.lock";
static int fd;

void empkg_unlock(void) {
	flock(fd, LOCK_UN);
}

int empkg_lock(void) {
	empkg_fops_mkdir("/var/lock");

	fd = open(empkg_file_lock, O_RDWR | O_CREAT, 0600);
	if (fd < 0) {
		fprintf(stderr, "Cannot open lock file\n");
		return 0;
	}

	if (flock(fd, LOCK_EX|LOCK_NB) == -1) {
		fprintf(stderr, "Another instance of this program is running.\n");
		return 0;
	}

	return 1;
}
