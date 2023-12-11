/*
 * empkg_enable.h - App enable/disable header
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

int empkg_enable(const char *id);
int app_enable(const char *id);
int empkg_disable(const char *id);
int app_disable(const char *id);
