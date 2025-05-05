/*
 * empkg_helper.h - helper function declarations
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

int empkg_is_arch_supported(const char *arch);
void empkg_reset_appdir(const char *sub);
char *remove_suffix(char *filename);
void empkg_init_dirs(void);
void empkg_request_daemon_reload(char *cmd, char *app);
int empkg_process_reload_request(void);
int empkg_update_www(void);
void empkg_update_licenses(void);
int empkg_update_firewall_single(const char *id);
int empkg_update_firewall_all_enabled(void);
