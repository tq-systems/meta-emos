/*
 * empkg_helper.h - helper function declarations
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

int empkg_is_arch_supported(const char *arch);
void empkg_reset_appdir(const char *sub);
char *remove_suffix(char *filename);
void empkg_init_dirs(void);
