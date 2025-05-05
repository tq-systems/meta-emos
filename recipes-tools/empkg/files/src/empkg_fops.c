/*
 * empkg_fops.c - File operations
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg.h" /* __maybe_unused */
#include "empkg_appdb.h"
#include "empkg_fops.h"
#include "empkg_log.h"

char *empkg_fops_abspath(const char *root, const char *relpath) {
	char *path;

	if (asprintf(&path, "%s/%s", root, relpath) == -1)
		return NULL;

	return path;
}

int empkg_fops_symlink(const char *target, const char *path) {
	int ret;
	char linktarget[APPDB_MAX_PATH];
	char *path_tmp;

	/* readlink() does not append a null byte to buf, so prefill with nulls */
	memset(linktarget, 0, APPDB_MAX_PATH);

	/* Where does symlink point to? */
	ret = readlink(path, linktarget, APPDB_MAX_PATH);
	if ((ret < 0) && (errno != ENOENT)) /* exit when something other than NOTFOUND happened */
		goto exit_symlink;
	else
		ret = 0;

	/* When symlink target and desired target differ, overwrite symlink atomically */
	if (strcmp(linktarget, target)) {
		if (asprintf(&path_tmp, "%s.tmp", path) == -1) {
			ret = ERRORCODE;
		} else {
			ret = symlink(target, path_tmp);
			rename(path_tmp, path); /* replaces path instantly */
			sync();
		}
		free(path_tmp);
	}

exit_symlink:
	if ((ret < 0) && (errno != ENOENT))
		log_message("empkg: Error creating symlink to %s at %s (%s)\n", target, path, strerror(errno));

	return ret;
}

static int unlink_callback(const char *fpath, __maybe_unused const struct stat *sb,
			   __maybe_unused int typeflag, __maybe_unused struct FTW *ftwbuf) {
	return remove(fpath);
}

int empkg_fops_rm(const char *path) {
	int ret = nftw(path, unlink_callback, 16, FTW_DEPTH | FTW_PHYS);

	if (ret && errno != ENOENT)
		log_message("empkg: Error removing %s (%s)\n", path, strerror(errno));

	return (errno == ENOENT ? 0 : ret);
}

int empkg_fops_mv(const char *path, const char *newpath) {
	int ret = rename(path, newpath);
	if (ret && errno == EXDEV) {
		/* Moving across filesystems, copying then deleting... */
		ret = empkg_fops_cp(path, newpath);
		if (!ret)
			empkg_fops_rm(path);
	}

	if (ret)
		log_message("empkg: Error moving %s to %s (%s)\n", path, newpath, strerror(errno));

	return ret;
}

int empkg_fops_chown(const char *path, uid_t owner, gid_t group) {
	struct stat sb;
	struct dirent **ent;
	int i = 0, n;
	char *subpath;
	int ret = lchown(path, owner, group);

	if (!ret)
		ret = lstat(path, &sb);

	if (!ret && S_ISDIR(sb.st_mode)) {
		n = scandir(path, &ent, scandir_filter_default, alphasort);

		while (!ret && (i < n)) {
			if (asprintf(&subpath, "%s/%s", path, ent[i]->d_name) == -1) {
				ret = ERRORCODE;
			} else {
				ret = empkg_fops_chown(subpath, owner, group);
				free(subpath);
			}
			free(ent[i]);
			i++;
		}
		free(ent);
	}

	if (ret)
		log_message("empkg: error chown() on '%s' (%s)\n", path, strerror(errno));

	return ret;
}

int empkg_fops_chown_name(const char *path, const char * const owner, const char * const group)
{
	struct passwd *pusr;
	struct group *pgrp;

	pusr = getpwnam(owner);
	pgrp = getgrnam(group);

	if (!pusr || !pgrp)
		return ERRORCODE;

	return empkg_fops_chown(path, pusr->pw_uid, pgrp->gr_gid);
}
