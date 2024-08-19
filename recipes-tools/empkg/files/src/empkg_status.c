/*
 * empkg_status.c - Status Command
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg.h"
#include "empkg_appdb.h"
#include "empkg_json.h"
#include "empkg_log.h"
#include "empkg_status.h"

/*
{
    "status": {
        "builtin": "true",
        "enabled": "true"
    },
    "info": {
        "id": "web-login",
        "version": "2.5.0",
        "arch": "aarch64",
        "essential": true,
        "variant": "tq",
        "name": "",
        "description": "",
        "lang": "tq"
    }
}
*/
static int status_one(const char *id, json_t **json) {
	bool builtin = false, enabled = false;
	const char *path = appdb_get_path(P_MANIFEST, id);

	if (!appdb_is(INSTALLED, id)) {
		log_message("empkg: app '%s' not found.\n", id);
		return ERRORCODE;
	}

	if (appdb_is(BUILTIN, id))
		builtin = true;

	if (appdb_is(ENABLED, id))
		enabled = true;

	*json = empkg_json_generate_status(builtin, enabled, path);

	return 0;
}

static int status_all_callback(const char *id) {
	int ret;
	json_t *json;

	ret = status_one(id, &json);
	if (!ret)
		ret = json_object_set(g_json_output, id, json);

	return ret;
}

int status(const char *id) {
	int ret;

	g_json_output = json_object();

	if (!id)
		ret = appdb_all(INSTALLED, status_all_callback);
	else
		ret = status_one(id, &g_json_output);

	/* stderr: manual command 'status' should output directly */
	fprintf(stderr, "%s\n", json_dumps(g_json_output, JSON_INDENT(2)));

	return ret;
}
