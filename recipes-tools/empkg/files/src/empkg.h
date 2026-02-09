/*
 * empkg.h - empkg main header
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#ifndef __EMPKG_H__
#define __EMPKG_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/mount.h>
#include <sys/utsname.h> /* uname */

#define __maybe_unused	__attribute__((__unused__))
#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))
#define ERRORCODE 1
#define ERRORDEFER 2
#define ERRORDEFERSILENT 3

#define gAPPDIR			"/apps"
#define gBUILTINDIR		"/opt/apps"
#define gINSTALLEDDIR		gAPPDIR"/installed"
#define gDBUSDIR		gAPPDIR"/dbus"
#define gENABLEDDIR		gAPPDIR"/enabled"
#define gCONFIGDIR		"/cfglog/apps"
#define gRUNDIRAPPS		"/run/em/apps"
#define gUSERDBRUNDIR		"/run/host/userdb"
#define gUSERDBSTOREDIR		gAPPDIR"/userdb"
#define gCONFIGSYSTEMDIR	"/cfglog/system"
#define gCONFIGAUTHDIR		"/cfglog/auth"
#define gDATAAPPSDIR		"/data/apps"

struct empkg_config {
	unsigned enable;
	unsigned systemd;
	unsigned eol;
	int (*cmd)(char *);
	char *arg;
};

extern struct empkg_config config;

struct empkg_cmd {
	char *cmd;
	int has_arg; /* 0 - no argument, 1 - required, 2 - optional */
	void *fn;
};

#endif /*__EMPKG_H__*/
