/*
 * empkg_acl.h - Header for access control list operations
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/acl.h> /* acl_... */

int empkg_acl_setacl(const char *path, const int uid, const bool write);
