/*
 * empkg_fops_cp.c - Recursivley copy files and dirs
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg_appdb.h"
#include "empkg_fops.h"

/* copy src file/dir to dst file/dir
 * if S_ISDIR() for src, mkdir dst -> fail if dst exists
 * 	scandir src/ and empkg_fops_cp(src/entry, dst/entry)
 * if S_ISREG() / S_ISLNK() for src, fread/fwrite
 */
int empkg_fops_cp(const char *src, const char *dst) {
	char *entrysrc, *entrydst;
	struct stat sb;
	int err;
	struct dirent **ent;
	int i = 0, n;
	FILE *fsrc, *fdst;
	unsigned char block[512];
	size_t bytes;

	err = lstat(src, &sb);
	if (!err && S_ISDIR(sb.st_mode)) {
		err = mkdir(dst, 0755);
		if (err) {
			fprintf(stderr, "Error copying: %s (%s)\n", dst, strerror(errno));
			return err;
		}

		n = scandir(src, &ent, scandir_filter_default, alphasort);

		while (!err && (i < n)) {
			if ((asprintf(&entrysrc, "%s/%s", src, ent[i]->d_name) == -1) ||
			    (asprintf(&entrydst, "%s/%s", dst, ent[i]->d_name) == -1))
				return ERRORCODE;

			err = empkg_fops_cp(entrysrc, entrydst);
			free(entrysrc);
			free(entrydst);
			free(ent[i]);
			i++;
		}
		free(ent);
	} else if (!err && S_ISREG(sb.st_mode)) {
		fdst = fopen(dst, "r");
		if (fdst) {
			fclose(fdst);
			fprintf(stderr, "Error copying: file %s exists\n", dst);
			errno = EEXIST;
			return ERRORCODE;
		}

		fdst = fopen(dst, "w");
		if (!fdst) {
			fprintf(stderr, "Error copying: could not open %s for writing (%s)\n", dst, strerror(errno));
			return ERRORCODE;
		}

		fsrc = fopen(src, "r");
		if (!fsrc) {
			fprintf(stderr, "Error copying: could not open %s for reading (%s)\n", src, strerror(errno));
			return ERRORCODE;
		}

		while (0 < (bytes = fread(block, 1, sizeof(block), fsrc)))
			fwrite(block, 1, bytes, fdst);

		fclose(fsrc);
		fclose(fdst);
	} else if (!err && S_ISLNK(sb.st_mode)) {
		entrydst = malloc(sb.st_size + 1);
		bytes = readlink(src, entrydst, sb.st_size);
		if (bytes == (size_t)sb.st_size) {
			entrydst[bytes] = '\0';
			empkg_fops_symlink(entrydst, dst);
			empkg_fops_rm(src);
		}
		free(entrydst);
	}
	return err;
}
