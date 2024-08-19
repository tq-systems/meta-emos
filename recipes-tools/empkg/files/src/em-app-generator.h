/*
 * empkg - em-app-generator.c
 * Copyright © 2020 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Matthias Schiffer
 */

#include <dirent.h>
#include <errno.h>
#include <jansson.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define APP_BEFORE_TARGET "em-app-before.target"
#define CORE_TARGET_WANTS "multi-user.target.wants"
#define APP_TIME_TARGET_WANTS "em-app-time.target.wants"
#define APP_NO_TIME_TARGET_WANTS "em-app-no-time.target.wants"

bool is_core_app(const char *app);
bool is_time_app(const char *app);
bool is_autostart(const char *app);
void symlink_wants(const char *app, const char *target);
void add_dependencies(const char *app);

void handle_app(const char *app);
int em_app_generator(int argc, char *argv[]);
