/*
 * empkg_tar.c - TAR archive functions
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg_tar.h"

char *empkg_tar_pkg_info(const char *path) {
	char *app_info;
	struct archive *a;
	struct archive_entry *entry;
	void *ptr;
	size_t size;
	int ret;

	a = archive_read_new();
	archive_read_support_filter_all(a);
	archive_read_support_format_all(a);
	ret = archive_read_open_filename(a, path, EMPKG_BUF_SIZE);
	if (ret != ARCHIVE_OK) {
		fprintf(stderr, "Error opening empkg '%s'\n", path);
		return NULL;
	}

	/* iterate over .tar file and find block for filename manifest.json */
	while (ret == ARCHIVE_OK) {
		ret = archive_read_next_header(a, &entry);
		/* read error or manifest found, exit loop */
		if ((ret != ARCHIVE_OK) ||
		    (strcmp(archive_entry_pathname(entry), "manifest.json") == 0))
			break;
	}

	/* Check result of iteration. Exit if manifest.json was not found */
	if ((ret != ARCHIVE_OK) || (strcmp(archive_entry_pathname(entry), "manifest.json"))) {
		fprintf(stderr, "File manifest.json not found in %s\n", path);
		return NULL;
	}

	size = archive_entry_size(entry); /* get size of manifest.json */
	app_info = malloc(size+1); /* allocate enough to place manifest.json + null-termination in memory */
	if (!app_info) {
		archive_read_close(a);
		return NULL;
	}
	ptr = app_info; /* pointer to place next data from TAR-block */

	while (1) {
		const void *buffer;
		size_t buffer_size;
		__LA_INT64_T  offset;

		ret = archive_read_data_block (a, &buffer, &buffer_size, &offset);

		if (ret == ARCHIVE_EOF)
			break;

		memcpy(ptr, buffer, buffer_size);
		ptr += buffer_size;
	}

	app_info[size] = '\0';

	archive_read_close(a);
	archive_read_free(a);

	return app_info;
}

void empkg_tar_pkg_extract(const char *path, const char *installdir) {
	struct archive *ar;
	struct archive *aw;
	struct archive_entry *entry;
	int ret;
	const int archive_flags = ARCHIVE_EXTRACT_OWNER |
				  ARCHIVE_EXTRACT_PERM |
				  ARCHIVE_EXTRACT_TIME |
				  ARCHIVE_EXTRACT_UNLINK |
				  ARCHIVE_EXTRACT_XATTR |
				  ARCHIVE_EXTRACT_SECURE_SYMLINKS;

	ar = archive_read_new();
	archive_read_support_filter_all(ar);
	archive_read_support_format_all(ar);

	aw = archive_write_disk_new();
	archive_write_disk_set_options(aw, archive_flags);

	ret = archive_read_open_filename(ar, path, EMPKG_BUF_SIZE);
	if (ret != ARCHIVE_OK) {
		fprintf(stderr, "Error opening empkg '%s'\n", path);
		return;
	}

	if(chdir(installdir) != 0) {
		fprintf(stderr, "Could not change to %s\n", installdir);
		return;
	}

	while (archive_read_next_header(ar, &entry) == ARCHIVE_OK) {
		const void *buff;
		size_t size;
		la_int64_t offset;

		/* create file */
		if (archive_write_header(aw, entry) < ARCHIVE_OK) {
			fprintf(stderr, "write: %s\n", archive_error_string(aw));
			continue;
		}

		if (archive_entry_size(entry) > 0) {
			/* extract file data */
			while (archive_read_data_block(ar, &buff, &size, &offset) != ARCHIVE_EOF) {
				if (archive_write_data_block(aw, buff, size, offset) < ARCHIVE_OK) {
					fprintf(stderr, "write: %s\n", archive_error_string(aw));
					break;
				}
			}
		}
	}

	archive_read_close(ar);
	archive_read_free(ar);
	archive_write_close(aw);
	archive_write_free(aw);
}
