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

const char *empkg_json_get_char(const char *id, const char *key);
int empkg_json_get_int(const char *id, const char *property);
char *empkg_json_pretty(const char *input);
json_t *empkg_json_generate_status(const bool builtin, const bool enabled, const char *path);
json_t *empkg_json_get_manifest_permissions(const char *id);
void empkg_json_append_note(json_t *json, const char *note);
