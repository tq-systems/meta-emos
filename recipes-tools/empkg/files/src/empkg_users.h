/*
 * empkg_users.h - Header of app user handling for systemd-nss
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include <jansson.h> /* json_... */
#include <libgen.h> /* dirname */
#include <sys/acl.h> /* acl_... */
#include <unistd.h>
#include <pwd.h> /* getpwnam() */
#include <grp.h> /* getgrnam() */

int empkg_users_sync_app_users_and_dirs(const char *id);
