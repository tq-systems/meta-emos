/*
 * empkg_acl.c - Access control list operations
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg_appdb.h"
#include "empkg_acl.h"
#include "empkg_log.h"

/* Find the ACL entry in 'acl' corresponding to the tag type and
 * qualifier in 'tag' and 'qual'. Return the matching entry, or NULL
 * if no entry was found.
 */
static acl_entry_t empkg_acl_find_aclentry(acl_t acl, uid_t uid)
{
	acl_entry_t entry;
	acl_tag_t tag_type;
	int entry_id = ACL_FIRST_ENTRY;
	uid_t *entry_uid;

	/* obtain first/next ACL entry */
	while(acl_get_entry(acl, entry_id, &entry) == 1) {
		entry_id = ACL_NEXT_ENTRY;

		/* find type ACL_USER entry */
		acl_get_tag_type(entry, &tag_type);
		if (tag_type != ACL_USER)
			continue;

		/* get qualifier(for ACL_USER entries that's the uid) */
		entry_uid = acl_get_qualifier(entry);
		if (!entry_uid) {
			log_message("empkg: Error getting entry uid.\n");
			return NULL;
		}

		/* Are these the droids we are looking for? */
		if (*entry_uid == uid) {
			acl_free(entry_uid);
			return entry;
		}
		acl_free(entry_uid);
	}

	/* none found */
	return NULL;
}

/* recursive wrapper for setacl()
 * If path is a symlink, it does not follow.
 * If path is a directory, it gets called recursively on the path contents.
 */
int empkg_acl_setacl_r(const char *path, const int uid, const bool write) {
	struct stat sb;
	int err;

	err = lstat(path, &sb);
	if (err) {
		log_message("empkg: Error accessing '%s'\n", path);
		return 0;
	}

	if (S_ISDIR(sb.st_mode)) {
		struct dirent **ent = NULL;
		char *workpath;
		int n, i = 0;

		n = scandir(path, &ent, scandir_filter_default, alphasort);

		while (i < n) {
			if(asprintf(&workpath, "%s/%s", path, ent[i]->d_name) == -1) {
				log_message("empkg: Error allocating memory\n");
				free(ent[i]);
				i++;
				continue;
			}

			empkg_acl_setacl_r(workpath, uid, write);
			free(workpath);
			free(ent[i]);
			i++;
		}
		free(ent);
	}

	/* dir entries done, handle other types and dir itself
	 * do not follow symlinks!
	 */
	if (!S_ISLNK(sb.st_mode))
		empkg_acl_setacl(path, uid, write);

	return 0;
}

int empkg_acl_setacl(const char *path, const int uid, const bool write) {
	acl_t acl;
	acl_entry_t aclentry;
	acl_permset_t aclpermset;
	int ret = 0;

	/* we want to actually modify the existing ACL
	 * or add a new entry for user with permissions
	 * but keep the other entries untouched
	 */

	/* get current ACL */
	acl = acl_get_file(path, ACL_TYPE_ACCESS);
	if (!acl) {
		log_message("empkg: Error getting ACL from '%s': %s.\n", path, strerror(errno));
		return -1;
	}

	aclentry = empkg_acl_find_aclentry(acl, uid);
	if (!aclentry) {
		/* prepare a new entry */
		if (acl_create_entry(&acl, &aclentry) == -1) {
			log_message("empkg: Error creating ACL entry.\n");
			acl_free(acl);
			return -1;
		}

		acl_set_tag_type(aclentry, ACL_USER);
		acl_set_qualifier(aclentry, &uid);
	}

	/* add permissions for user */
	acl_get_permset(aclentry, &aclpermset);
	acl_add_perm(aclpermset, ACL_READ | ACL_EXECUTE);
	if (write)
		acl_add_perm(aclpermset, ACL_WRITE);

	/* update ACL_MASK to accomodate all current permissions */
	if (acl_calc_mask(&acl))
		log_message("empkg: Error calculating mask\n");

	if (acl_valid(acl) == 0) {
		ret = acl_set_file(path, ACL_TYPE_ACCESS, acl);
		if (ret)
			log_message("empkg: Error assigning ACL to '%s': %s.\n", path, strerror(errno));
	} else {
		log_message("empkg: Error validating ACL:\n%s\n", acl_to_text(acl, NULL));
	}

	acl_free(aclpermset);
	acl_free(aclentry);
	acl_free(acl);

	return ret;
}
