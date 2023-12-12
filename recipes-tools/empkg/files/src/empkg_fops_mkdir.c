/*
 * empkg_fops_mkdir.c - Recursivly creates directories with all parents
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg.h"
#include "empkg_fops.h"

/* mode = 0755 */
#define MKDIRMODE (S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)

static inline int empkg_do_mkdir(const char *path, mode_t mode) {
	int ret;
	ret = mkdir(path, mode);
	if (errno != EEXIST)
		printf("Error creating dir %s (%s)\n", path, strerror(errno));
	return ret;
}

/* implementation of 'mkdir -p', no error if directories exist */
int empkg_fops_mkdir(const char *path) {
	char *workpath;
	char *curptr;
	size_t len;
	int ret;

	if (asprintf(&workpath, "%s", path) == -1)
		return ERRORCODE;

	len = strlen(workpath) - 1;

	if (workpath[len] == '/')
		workpath[len] = '\0';

	curptr = workpath + 1;
	while(*curptr) {
		if (*curptr == '/') {
			*curptr = '\0';
			ret = empkg_do_mkdir(workpath, MKDIRMODE);
			*curptr = '/';
		}
		curptr++;
	}

	ret = empkg_do_mkdir(workpath, MKDIRMODE);
	if (ret && errno == EEXIST)
		ret = errno = 0;

	free(workpath);

	return ret;
}
