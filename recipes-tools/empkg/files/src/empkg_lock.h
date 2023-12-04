/*
 * empkg_lock.h - File lock header
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include <stdio.h>
#include <sys/file.h>
#include <unistd.h>

void empkg_unlock(void);
int empkg_lock(void);
