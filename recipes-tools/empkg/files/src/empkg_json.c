/*
 * empkg_json.c - JSON conversions
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg.h" /* gAPPDIR, gBUILTINDIR */
#include "empkg_appdb.h"
#include "empkg_json.h"

json_t *g_json_output;

const char *empkg_json_get_char(const char *app, const char *key) {
	json_t *json;
	const char *path = appdb_get_path(P_MANIFEST, app);
	char *retval;

	json = json_load_file(path, 0, NULL);

	if (json && json_is_object(json)) {
		json_t *value = json_object_get(json, key);
		if (!value) {
			json_decref(json);
			return NULL;
		}

		retval = strdup(json_string_value(value));
		json_decref(json);
		return retval;
	}

	json_decref(json);
	return NULL;
}

/* errors must return negative values to differentiate from valid booleans */
int empkg_json_get_int(const char *id, const char *property) {
	json_t *json;
	const char *path = appdb_get_path(P_MANIFEST, id);

	json = json_load_file(path, 0, NULL);
	if (!json)
		return -1;

	if (json_is_object(json)) {
		json_t *value = json_object_get(json, property);

		if (!value)
			return -1;

		return json_boolean_value(value);
	}

	json_decref(json);
	return -1;
}

json_t *empkg_json_generate_status(const bool builtin, const bool enabled, const char *path) {
	json_t *json = json_load_file(path, 0, NULL);

	if (!json)
		return NULL;

	if (json_is_object(json)) {
		json_t *root = json_object();
		json_t *status = json_object();

		json_object_set_new(status, "builtin", json_boolean(builtin));
		json_object_set_new(status, "enabled", json_boolean(enabled));

		json_object_set_new(root, "status", status);
		json_object_set_new(root, "info", json);

		return root;

		/* json_decref(status); */
		/* json_decref(root); */
	}

	json_decref(json);

	return json;
}
