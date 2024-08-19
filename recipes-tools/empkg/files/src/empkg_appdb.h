/*
 * empkg_appdb.h - app database header
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */
#ifndef __EMPKG_APPDB_H__
#define __EMPKG_APPDB_H__

#include <ctype.h>
#include <dirent.h>
#include <errno.h>/* Error macros */
#include <jansson.h>
#include <openssl/evp.h>
#include <stdbool.h>
#include <sys/stat.h> /* struct stat */
#include "empkg.h" /* gAPPDIR, gBUILTINDIR */

/* The app database consists of 5 types of functions:
 * appdb_scan_*(void) - database initiation
 * ----------------------------------------
 * Available for:
 *    *_builtin    -  Assumes all entries found in the rootfs installation directory
 *                    (installed via rauc bundle or production) as app and sets their "builtin" property.
 *                    If not found, creates new entry with all other properties set to APPDB_INIT_VAL.
 *                    Default path: /opt/apps.
 *
 *    *_installed  -  Assumes all entries found in the /apps partition installation directory
 *                    (installed via em-app-install) as app and sets their "installed" property.
 *                    If not found, creates new entry with all other properties set to APPDB_INIT_VAL.
 *                    Default path: /apps/installed.
 *
 *    *_enabled    -  Assumes all symlinks found in the /apps/enabled directory as app and sets
 *                    their "enabled" property.
 *                    If not found, creates new entry with all other properties set to APPDB_INIT_VAL.
 *                    Default path: /apps/enabled.
 *
 * appdb_get/set(property, app id, [value]) - database accessors
 * -------------------------------------------------------------
 * Used to access value of property for the app.
 * If the value still is APPDB_INIT_VAL, get() calls appdb_check().
 * Separate get()-implementations for properties VERSION and MD5SUMS, because different return values.
 * Separate set()-implementation for VERSION, because of different type for value.
 * For MD5SUMS, appdb_check_md5sums() must be used, because the property is determined from files.
 *
 * appdb_check(property, app id) - property determination
 * ------------------------------------------------------
 * Used to determine value of the property.
 * Depending on the property this could mean to check the presence of a certain symlink/directory or
 * a find a JSON key:value pair in the app manifest.
 *
 * appdb_is(property, app id) - property testing
 * ---------------------------------------------
 * Wrapper to use in conditions to test if property is set for app id
 * Separate implementation
 *    appdb_is_valid_id(name) - Tests if name consists of only numbers, small letters or dash('-')
 *
 * appdb_all(property, callback function(app id))
 * ----------------------------------------------
 * Iterates over appdb through all entries with the property set and calls the callback function
 * with the app id of the entry as parameter.
 */

enum {
	INDEX,
	LANG,
	NSUMS
};

typedef unsigned char md5sums_t[16];

/* Paths enum */
enum {
	P_BUILTIN,
	P_CONFIGDIR,
	P_DATADIR,
	P_DBUSAPP,
	P_DBUSCONF,
	P_ENABLED,
	P_INDEX,
	P_INSTALLED,
	P_LANG,
	P_MANIFEST,
	P_RUNDIR,
	P_SERVICE,
	P_WWW,
	NPATHS
};

#define APPDB_MAX_PATH 128

extern const char pattern[NPATHS][APPDB_MAX_PATH];

/* global app database:
 * struct appdb_t[] = {
 * char *id,        --> space-free, path-compatible name
 * char *name,      --> pretty name for the app
 * char *version,
 * int valid_name,  --> echo "$1" | grep -qE '^[a-z0-9-]+$'
 * int builtin,     --> is_valid_name "$1" && [ -d "$BUILTIN_DIR/$1" ]
 * int enabled,     --> is_valid_name "$1" && [ -L "$APPDIR/enabled/$1" ]
 * int installed,   --> is_valid_name "$1" && [ -d "$APPDIR/installed/$1" ]
 * int autostart,   --> is_enabled "$app" && app_info "$app" | jq -e '.autostart != false'
 * int essential,   --> is_builtin "$app" && app_info "$app" | jq -e '.essential'
 * int systemd,     --> is_installed "$1" && [ -e "$APPDIR/installed/$1/em-app-$1.service" ]
 * int disabled,    --> app_info "$app" | jq -e '.disabled == true'
 * md5sums_t md5sums --> holds md5sums for /apps/www/apps.json and /apps/www/langs.json
 * },
 */
struct appdb_t {
	char *id;
	char *name;
	char *version;
	int valid_name;
	int autostart;
	int builtin;
	int disabled;
	int enabled;
	int essential;
	int systemd;
	int installed;
	char paths[NPATHS][APPDB_MAX_PATH];
	md5sums_t md5sums[NSUMS];
	struct appdb_t *next;
};

/* This info of empkg files is needed for app installation:
 * char *name      --> taken from --> id field in manifest.json
 * char *version   --> taken from --> version to be installed
 * char *arch      --> taken from --> architecture the app is compatible with
 */
struct empkg_new_t {
	const char *id;
	const char *version;
	const char *arch;
};

typedef enum appdb_prop {
	AUTOSTART,
	BUILTIN,
	DISABLED,
	ENABLED,
	ESSENTIAL,
	INSTALLED,
	SYSTEMD,
	VERSION,
	MD5SUMS,
} appdb_prop_t;
typedef appdb_prop_t dbp;

int appdb_scan_installed(void);
int appdb_scan_builtin(void);
int appdb_scan_enabled(void);

void appdb_check(const dbp prop, const char *id);

char *appdb_get_path(const int path, const char *id);
int appdb_get(const dbp prop, const char *id);
char *appdb_get_version(const char *id);
md5sums_t *appdb_get_md5sums(const char *id);

int appdb_set(const dbp prop, const char *id, const int value);
void appdb_set_version(const char *id, const char *value);

int appdb_all(const dbp prop, int (*callback_fn)(const char *));
bool appdb_is_valid_id(const char *id);
#define appdb_is(p, a) (appdb_get(p, a) == 1)

int scandir_filter_default(const struct dirent *ent);
int scandir_filter_valid_id(const struct dirent *ent);
int scandir_filter_valid_id_link(const struct dirent *ent);

#endif /* __EMPKG_APPDB_H__ */
