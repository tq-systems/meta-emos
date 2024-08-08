/*
 * empkg_json.h - Header for JSON conversions
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include <ctype.h>
#include <errno.h>/* Error macros */
#include <jansson.h>
#include <stdbool.h>

extern json_t *g_json_output;

char *empkg_json_get_char(const char *id, const char *key);
int empkg_json_get_int(const char *id, const char *property);
