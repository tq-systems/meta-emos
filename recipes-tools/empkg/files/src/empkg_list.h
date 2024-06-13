/*
 * empkg_list.h - Header for List commands
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "jansson.h"

extern struct empkg_config config;

int list_installed(void);
int list_enabled(void);
int list_autostart(void);
int list_apps(void);
int is_installed(const char *id);
int is_enabled(const char *id);
