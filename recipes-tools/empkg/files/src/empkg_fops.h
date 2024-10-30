/*
 * empkg_fops.h - Header for file operations
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include <ctype.h>
#include <dirent.h>
#include <errno.h> /* errno */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h> /* mkdir */
#include <unistd.h> /* symlink */
#include <pwd.h>
#include <grp.h>

char *empkg_fops_abspath(const char *root, const char *relpath);
int empkg_fops_symlink(const char *target, const char *path);
int empkg_fops_rm(const char *path);
int empkg_fops_cp(const char *src, const char *dst);
int empkg_fops_mv(const char *path, const char *newpath);
int empkg_fops_chown(const char *path, uid_t owner, gid_t group);
int empkg_fops_chown_name(const char *path, const char * const owner, const char * const group);
int empkg_fops_chmod(const char * const path, mode_t mode);
int empkg_fops_mkdir(const char *path);
