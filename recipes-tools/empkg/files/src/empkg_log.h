/*
 * empkg_log.h - Function to write to kernel log
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#ifndef __EMPKG_LOG_H__
#define __EMPKG_LOG_H__

#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void log_message(const char *format, ...);

#endif
