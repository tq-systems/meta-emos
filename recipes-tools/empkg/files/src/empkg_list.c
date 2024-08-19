/*
 * empkg_list.c - List commands
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg.h"
#include "empkg_appdb.h"
#include "empkg_json.h"
#include "empkg_list.h"

static int printappversion(const char *id) {
	/* Output required on stdout, because read back from app */
	fprintf(stdout, "%s - %s\n", id, appdb_get_version(id));

	return 0;
};

static int empkg_json_array_append(const char *id) {
	return json_array_append(g_json_output, json_string(id));
}

static char *empkg_json_output_dumps(void) {
	return json_dumps(g_json_output, JSON_SORT_KEYS|JSON_INDENT(2));
}

int list_installed(void) {
	char *output;

	g_json_output = json_array();
	appdb_all(INSTALLED, empkg_json_array_append);

	output = empkg_json_output_dumps();
	fprintf(stdout, "%s\n", output);
	free(output);

	json_decref(g_json_output);

	return 0;
};

int list_enabled(void) {
	char *output;

	g_json_output = json_array();
	appdb_all(ENABLED, empkg_json_array_append);

	output = empkg_json_output_dumps();
	/* Output required on stdout, because read back from app */
	fprintf(stdout, "%s\n", output);
	free(output);

	json_decref(g_json_output);

	return 0;
};

int list_autostart(void) {
	char *output;

	g_json_output = json_array();
	appdb_all(AUTOSTART, empkg_json_array_append);

	output = empkg_json_output_dumps();
	fprintf(stdout, "%s\n", output);
	free(output);

	json_decref(g_json_output);

	return 0;
};

int list_apps(void) {
	appdb_all(INSTALLED, printappversion);

	return 0;
};

int is_installed(const char *id) {
	if (appdb_is(INSTALLED, id))
		return 0;
	else
		return ERRORCODE;
};

int is_enabled(const char *id) {
	if (appdb_is(ENABLED, id))
		return 0;
	else
		return ERRORCODE;
};
