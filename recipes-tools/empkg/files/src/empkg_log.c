/*
 * empkg_log.c - Function to write to kernel log
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "empkg_log.h"

/* write log message to stderr and also to kernel log (requires root) */
void log_message(const char *format, ...) {
	FILE *kmsg;
	va_list args;
	va_start(args, format);

	// Check if the process is running as root (UID 0)
	if (geteuid() == 0) {
		kmsg = fopen("/dev/kmsg", "a");
		if (kmsg != NULL) {
			vfprintf(kmsg, format, args);
			fclose(kmsg);
		} else {
			fprintf(stderr, "Error: Unable to open /dev/kmsg\n");
		}
	}

	vfprintf(stderr, format, args);

	va_end(args);
}
