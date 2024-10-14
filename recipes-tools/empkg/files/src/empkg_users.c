/*
 * empkg_users.c - App user handling for systemd-nss
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg.h" /* gAPPDIR, gBUILTINDIR */
#include "empkg_acl.h"
#include "empkg_appdb.h"
#include "empkg_fops.h"
#include "empkg_json.h"
#include "empkg_log.h"
#include "empkg_users.h"

static int create_user_entry(const char *id, const int userid,
			     const int groupid, const char *userfile) {
	char *userdbrunfile, *userdbrunid;
	json_t *root = json_object();
	FILE *fuser;

	if ((asprintf(&userdbrunfile, "%s/%s", gUSERDBRUNDIR, userfile) == -1) ||
	    (asprintf(&userdbrunid, "%s/%d.user", gUSERDBRUNDIR, userid) == -1))
		return ERRORCODE;

	fuser = fopen(userdbrunfile, "w");
	if (!fuser) {
		log_message("empkg: Cannot open %s (%s)\n", userdbrunfile, strerror(errno));
		free(userdbrunfile);
		return errno;
	}
	free(userdbrunfile);

	json_object_set(root, "userName", json_string(id));
	json_object_set(root, "uid", json_integer(userid));
	json_object_set(root, "gid", json_integer(groupid));
	json_object_set(root, "disposition", json_string("reserved"));
	json_object_set(root, "locked", json_string("true"));
	json_dumpf(root, fuser, JSON_COMPACT);
	fclose(fuser);

	empkg_fops_rm(userdbrunid);
	empkg_fops_symlink(userfile, userdbrunid);
	free(userdbrunid);

	return 0;
}

static int create_group_entry(const char *groupname, const int groupid, const char *groupfile) {
	char *groupdbrunfile, *groupdbrunid;
	json_t *root = json_object();
	FILE *fgroup;

	if ((asprintf(&groupdbrunfile, "%s/%s", gUSERDBRUNDIR, groupfile) == -1) ||
	    (asprintf(&groupdbrunid, "%s/%d.group", gUSERDBRUNDIR, groupid) == -1))
		return ERRORCODE;

	fgroup = fopen(groupdbrunfile, "w");
	if (!fgroup) {
		log_message("empkg: Cannot open %s (%s)\n", groupdbrunfile, strerror(errno));
		return errno;
	}
	free(groupdbrunfile);

	json_object_set(root, "groupName", json_string(groupname));
	json_object_set(root, "gid", json_integer(groupid));
	json_object_set(root, "disposition", json_string("reserved"));
	json_dumpf(root, fgroup, JSON_COMPACT);
	fclose(fgroup);

	empkg_fops_rm(groupdbrunid);
	empkg_fops_symlink(groupfile, groupdbrunid);
	free(groupdbrunid);

	return 0;
}

static int get_free_user_group_id() {
	int id = 10000, stopid = 11000;
	struct stat sbu, sbg;
	char *userid, *groupid;

	while (id < stopid) {
		if ((asprintf(&userid, "%s/%d.user", gUSERDBRUNDIR, id) == -1) ||
		    (asprintf(&groupid, "%s/%d.group", gUSERDBRUNDIR, id) == -1))
			return ERRORCODE;

		if (stat(userid, &sbu) && stat(groupid, &sbg)) {
			/* stat returns -1 because userid/groupid not found */
			free(userid);
			free(groupid);
			break;
		} else {
			/* files userid or groupid found */
			free(userid);
			free(groupid);
			id++;
		}
	}

	return id;
}

static int get_gid_from_group_entry(const char *groupdb) {
	json_t *json = json_load_file(groupdb, 0, NULL);
	int ret = -1;
	const char *key;
	const json_t *value;

	json_object_foreach(json, key, value)
		if (!strcmp(key, "gid"))
			ret = json_integer_value(value);

	return ret;
}

/* Currently unused */
/*
static int get_uid_from_user_entry(const char *userdb) {
	json_t *json = json_load_file(userdb, 0, NULL);
	int ret = -1;
	const char *key;
	const json_t *value;

	json_object_foreach(json, key, value)
		if (!strcmp(key, "uid"))
			ret = json_integer_value(value);

	return ret;
}
*/

static int get_user_from_sdfile(char **user, const char *sdfile) {
	FILE *fsd;
	char line[512], *name;

	/* if no user is set in sdfile or no sdfile exists, assume root */
	*user = strdup("root");

	fsd = fopen(sdfile, "r");
	if (fsd) {
		/* scan through file for lines beginning with 'User=' and assign contents after = */
		while (fgets(line, sizeof(line), fsd)) {
			if (strstr(line, "User=") == line) {
				if (line[strlen(line) - 1] == '\n')
					line[strlen(line) - 1] = '\0'; /* remove newline character */

				name = line;
				strsep(&name, "="); /* point name past '=' */
				*user = strdup(name);
			}
		}
		fclose(fsd);
	}

	return 0;
}

static int get_group_from_sdfile(char **group, const char *sdfile, const char *user) {
	FILE *fsd;
	char line[512], *name;

	/* Group equals user if no group is set */
	*group = strdup(user);

	fsd = fopen(sdfile, "r");
	if (fsd) {
		while (fgets(line, sizeof(line), fsd)) {
			if (strstr(line, "Group=") == line) {
				if (line[strlen(line) - 1] == '\n')
					line[strlen(line) - 1] = '\0'; /* remove newline character */

				name = line;
				strsep(&name, "="); /* point name past '=' */
				*group = strdup(name);
			}
		}
		fclose(fsd);
	}

	return 0;
}

static int sync_app_user(const char *id, const char *user, const char *group) {
	char *groupfile, *groupdb, *userfile, *userdb;
	int groupid, userid;
	struct stat sb;
	int err = 0;

	if ((asprintf(&groupfile, "%s.group", group) == -1) ||
	    (asprintf(&userfile, "%s.user", user) == -1) ||
	    (asprintf(&groupdb, "%s/%s", gUSERDBRUNDIR, groupfile) == -1) ||
	    (asprintf(&userdb, "%s/%s", gUSERDBRUNDIR, userfile) == -1))
		return ERRORCODE;

	/* User entry always needs a groupid, so it is always handled _after_ the group entry */
	if (strcmp(group, "root")) {
		err = lstat(groupdb, &sb);
		if (err && errno == ENOENT) {
			groupid = get_free_user_group_id();
			err = create_group_entry(group, groupid, groupfile);
		} else {
			groupid = get_gid_from_group_entry(groupdb);
		}

	} else {
		/* Set gid for root */
		groupid = 0;

		/* Removing groups/group-ids from persistent storage is not supported for now
		 * in order to protect against reassignment of group-ids from uninstalled to new apps
		 * and the inheritance of corresponding file permissions tied to these ids.
		 */
	}

	if (err || groupid < 0)
		log_message("empkg: Error getting groupid for '%s'\n", id);

	if (strcmp(user, "root")) {
		if (strcmp(user, group) == 0)
			userid = groupid;
		else
			userid = get_free_user_group_id();

		err = lstat(userdb, &sb);
		if (err && errno == ENOENT)
			err = create_user_entry(user, userid, groupid, userfile);
	}

	free(groupfile);
	free(userfile);
	free(groupdb);
	free(userdb);

	return err;
}

static int sync_app_dirs(const char *id, const char *user, const char *group) {
	const char *rundirapp = appdb_get_path(P_RUNDIR, id);
	const char *configdirapp = appdb_get_path(P_CONFIGDIR, id);
	mode_t oldmask;
	struct passwd *pusr = getpwnam(user);
	struct passwd usernam, userwww;
	struct group *pgrp = getgrnam(group);
	struct group groupnam;
	int ret = 0;

	oldmask = umask(0007);
	empkg_fops_mkdir(rundirapp);

	umask(0077);
	empkg_fops_mkdir(configdirapp);

	if (!pusr || !pgrp) {
		log_message("empkg: Error collecting uid for '%s' or gid for '%s'.\n", user, group);
		return ERRORCODE;
	}

	memcpy(&usernam, pusr, sizeof(struct passwd));
	memcpy(&groupnam, pgrp, sizeof(struct group));

	pusr = getpwnam("www");
	memcpy(&userwww, pusr, sizeof(struct passwd));

	/* setfacl -m 'u:www:x' "$RUNDIRAPPS/$app" */
	empkg_acl_setacl(rundirapp, userwww.pw_uid, false);

	umask(oldmask);

	/* recursive chown */
	ret = empkg_fops_chown(configdirapp, usernam.pw_uid, groupnam.gr_gid);
	if (!ret)
		ret = empkg_fops_chown(rundirapp, usernam.pw_uid, groupnam.gr_gid);

	return ret;
}

static int empkg_users_handle_permissions(const char *id, const char *user, const char *group) {
	json_t *json_perm, *dir;
	DIR *dircheck;
	size_t notused;
	struct passwd *pusr = getpwnam(user);
	struct passwd usernam, userwww;
	struct group *pgrp = getgrnam(group);
	struct group groupnam;
	int ret = 0;

	json_perm = empkg_json_get_manifest_permissions(id);

	if (!pusr || !pgrp) {
		log_message("empkg: Error collecting uid for '%s' or gid for '%s'.\n", user, group);
		return ERRORCODE;
	}

	/* getpwnam points to a static memory that gets updated on each call.
	 * we need to copy the memory contents to store the results before
	 * subsequent calls (for www user later on)
	 */
	memcpy(&usernam, pusr, sizeof(struct passwd));
	memcpy(&groupnam, pgrp, sizeof(struct group));

	json_array_foreach(json_object_get(json_perm, "own"), notused, dir) {
		const char *dirpath = json_string_value(dir);

		if (!strcmp(appdb_get_path(P_RUNDIR, id), dirpath) ||
		    !strcmp(appdb_get_path(P_CONFIGDIR, id), dirpath) ||
		    !strcmp(appdb_get_path(P_DATADIR, id), dirpath)) {
			/* create own directory */
			empkg_fops_mkdir(dirpath);

			empkg_fops_chown(dirpath, usernam.pw_uid, groupnam.gr_gid);

			/* also grant execution access for nginx user 'www'
			 * to /run/apps/$app to display frontend
			 */
			if (!strcmp(appdb_get_path(P_RUNDIR, id), dirpath)) {
				pusr = getpwnam("www");
				if (pusr) {
					memcpy(&userwww, pusr, sizeof(struct passwd));
					/* setfacl -m "u:www:x" "$dir" */
					empkg_acl_setacl(appdb_get_path(P_RUNDIR, id), userwww.pw_uid, false);
				}
			}
		} else {
			log_message("empkg: %s: Cannot create own-group directory: '%s' is no own directory.\n", id, dirpath);
		}
	}

	/* keep empkg backwards compatible to apps without path permissions in manifest */
	if (json_array_size(json_object_get(json_perm, "own")) == 0) {
		log_message("empkg: Creating own directories (legacy mode). Please update %s to path permissions.\n", id);
		sync_app_dirs(id, user, group);
	}

	/* ReadWritePath entries */
	json_array_foreach(json_object_get(json_perm, "rw"), notused, dir) {
		const char *dirpath = json_string_value(dir);
		dircheck = opendir(dirpath);
		if (dircheck) {
			/* setfacl -Rm "u:$user:rwx" "$dir" */
			empkg_acl_setacl_r(dirpath, usernam.pw_uid, true);
		} else {
			log_message("empkg: %s: Cannot assign rw access: '%s' does not exist.\n", id, dirpath);
			ret = ERRORDEFER;
		}
		closedir(dircheck);
	}

	if (ret)
		return ret;

	/* ReadOnlyPath entries */
	json_array_foreach(json_object_get(json_perm, "ro"), notused, dir) {
		const char *dirpath = json_string_value(dir);
		dircheck = opendir(dirpath);
		if (dircheck) {
			/* setfacl -Rm "u:$user:r-x" "$dir" */
			ret = empkg_acl_setacl_r(dirpath, usernam.pw_uid, false);
		} else {
			log_message("empkg: %s: Cannot assign ro access: '%s' does not exist.\n", id, dirpath);
			ret = ERRORDEFER;
		}
		closedir(dircheck);
	}

	return ret;
}

int empkg_users_sync_app_users_and_dirs(const char *id) {
	const char *sdfile = appdb_get_path(P_SERVICE, id);
	char *user, *group;
	int ret;

	if (!appdb_is(SYSTEMD, id))
		return 0;

	ret = get_user_from_sdfile(&user, sdfile);

	ret |= get_group_from_sdfile(&group, sdfile, user);

	if (!ret)
		ret = sync_app_user(id, user, group);

	if (!ret)
		ret = empkg_users_handle_permissions(id, user, group);

	free(user);
	free(group);

	if (ret == ERRORDEFER) {
		log_message("empkg: Deferring %s\n", id);
		appdb_set(DEFERRED, id, 1);
		return 0;
	} else if (ret == 0) {
		appdb_set(DEFERRED, id, 0);
	}

	return ret;
}
