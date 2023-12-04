/*
 * empkg_tar.h - TAR archive function header
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include <archive.h>
#include <archive_entry.h>
#include <stdlib.h> /* malloc */
#include <string.h> /* strcmp */

#define EMPKG_BUF_SIZE 1024

/* returns content of manifest.json of an empkg file */
char *empkg_tar_pkg_info(const char *path);
