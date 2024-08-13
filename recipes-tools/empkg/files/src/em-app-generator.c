/*
 * empkg - em-app-generator.c
 * Copyright © 2020 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Matthias Schiffer
 */

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <errno.h>
#include <jansson.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define APP_BEFORE_TARGET "em-app-before.target"

#define CORE_TARGET_WANTS "multi-user.target.wants"
#define APP_TIME_TARGET_WANTS "em-app-time.target.wants"
#define APP_NO_TIME_TARGET_WANTS "em-app-no-time.target.wants"

const char *const CORE_APPS[] = {
	"backup",
	"button-handler",
	"devel",
	"device-settings",
	"health-check",
	"upnp",
	"web-login",
	NULL,
};

const char *const TIME_APPS[] = {
	"cloud-sync",
	"datalogger",
	"eventlogger",
	"evse-etrel",
	"evse-keba",
	"kostal-solar-electric",
	"livelogger",
	"modbus-daemon",
	"sensors",
	"tariffs",
	"teridiand",
	NULL,
};

char *empkg_json_get_char(const char *app, const char *key) {
	const char *manifest_pattern = "/apps/installed/%s/manifest.json";
	json_t *json;
	char *retval;

	char manifest_path[strlen(manifest_pattern) + strlen(app) + 1];
	snprintf(manifest_path, sizeof(manifest_path), manifest_pattern, app);

	json = json_load_file(manifest_path, 0, NULL);

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

bool is_core_app(const char *app) {
	size_t i;

	/* parse app manifest for core_app characteristic */
	const char *appclass = empkg_json_get_char(app, "appclass");

	if (appclass && strlen(appclass) > 0) {
		if (strcmp(appclass, "core") == 0)
			return true;

		return false;
	}

	fprintf(stderr, "Property 'appclass' not found in manifest. Please update %s by 2025.\n", app);

	for (i = 0; CORE_APPS[i]; i++) {
		if (strcmp(app, CORE_APPS[i]) == 0)
			return true;
	}

	return false;
}

bool is_time_app(const char *app) {
	size_t i;

	/* parse app manifest for time_app characteristic */
	const char *appclass = empkg_json_get_char(app, "appclass");

	if (appclass && strlen(appclass) > 0) {
		if (strcmp(appclass, "time") == 0)
			return true;

		return false;
	}

	for (i = 0; TIME_APPS[i]; i++) {
		if (strcmp(app, TIME_APPS[i]) == 0)
			return true;
	}

	return false;
}

bool is_autostart(const char *app) {
	const char *manifest_pattern = "/apps/installed/%s/manifest.json";
	json_t *json;
	bool ret = false;

	char manifest_path[strlen(manifest_pattern) + strlen(app) + 1];
	snprintf(manifest_path, sizeof(manifest_path), manifest_pattern, app);

	json = json_load_file(manifest_path, 0, NULL);
	if (!json)
		return false;

	if (json_is_object(json)) {
		json_t *autostart = json_object_get(json, "autostart");
		if (autostart) {
			ret = json_boolean_value(autostart);
		} else {
			ret = true;
		}
	}

	json_decref(json);

	return ret;
}

void symlink_wants(const char *app, const char *target) {
	const char *wants_pattern = "%s/em-app-%s.service";

	char wants_path[strlen(wants_pattern) + strlen(target) + strlen(app) + 1];
	char wants_target[strlen(wants_pattern) + 2 + strlen(app) + 1];

	snprintf(wants_path, sizeof(wants_path), wants_pattern, target, app);
	snprintf(wants_target, sizeof(wants_target), wants_pattern, "..", app);

	if (symlink(wants_target, wants_path)) {
		perror("symlink");
		return;
	}
}

void add_dependencies(const char *app) {
	const char *snippet_dir_pattern = "em-app-%s.service.d";
	const char *snippet_pattern = "em-app-%s.service.d/em-app-deps.conf";

	char snippet_dir[strlen(snippet_dir_pattern) + strlen(app) + 1];
	char snippet[strlen(snippet_pattern) + strlen(app) + 1];
	FILE *f;

	snprintf(snippet_dir, sizeof(snippet_dir), snippet_dir_pattern, app);
	snprintf(snippet, sizeof(snippet), snippet_pattern, app);

	if (mkdir(snippet_dir, 0777) && errno != EEXIST) {
		perror("mkdir");
		return;
	}

	f = fopen(snippet, "w");
	if (!f) {
		perror("fopen");
		return;
	}

	fprintf(f, "[Unit]\n");
	fprintf(f, "Requires=%s\n", APP_BEFORE_TARGET);
	fprintf(f, "After=%s\n", APP_BEFORE_TARGET);

	fclose(f);
}

void handle_app(const char *app) {
	const char *service_pattern = "/apps/installed/%s/em-app-%s.service";

	char service_path[strlen(service_pattern) + 2*strlen(app) + 1];
	char *service_name;

	snprintf(service_path, sizeof(service_path), service_pattern, app, app);
	service_name = basename(service_path);

	if (access(service_path, F_OK)) // App has no service file
		return;

	// Link unit so that it can be started via systemd
	if (symlink(service_path, service_name)) {
		perror("symlink");
		return;
	}

	if (!is_autostart(app))
		return;

	if (is_core_app(app)) {
		symlink_wants(app, CORE_TARGET_WANTS);
		return;
	}

	if (is_time_app(app)) {
		symlink_wants(app, APP_TIME_TARGET_WANTS);
	} else {
		symlink_wants(app, APP_NO_TIME_TARGET_WANTS);
	}

	add_dependencies(app);
}


int main(int argc, char *argv[]) {
	DIR *dir;
	struct dirent *ent;

	if (argc < 2) {
		fprintf(stderr, "em-app-generator: This executable is meant to be run as a generator by systemd.");
		return 1;
	}

	if (chdir(argv[1])) {
		perror("chdir");
		return 1;
	}

	if (mkdir(CORE_TARGET_WANTS, 0777) && errno != EEXIST) {
		perror("mkdir");
		return 1;
	}
	if (mkdir(APP_TIME_TARGET_WANTS, 0777) && errno != EEXIST) {
		perror("mkdir");
		return 1;
	}
	if (mkdir(APP_NO_TIME_TARGET_WANTS, 0777) && errno != EEXIST) {
		perror("mkdir");
		return 1;
	}

	dir = opendir("/apps/enabled");
	if (!dir) {
		perror("opendir");
		return 1;
	}

	while ((ent = readdir(dir)) != NULL) {
		if (ent->d_type != DT_LNK)
			continue;

		if (ent->d_name[0] == '.')
			continue;

		handle_app(ent->d_name);
	}

	closedir(dir);

	return 0;
}
