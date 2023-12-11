/*
 * empkg - EM Package Manager Tool
 *
 * Copyright © 2024 TQ-Systems GmbH <info@tq-group.com>
 * All rights reserved. For further information see LICENSE.
 * Author: Michael Krummsdorf
 */

#include "em-app-generator.h"
#include "empkg.h"
#include "empkg_appdb.h"
#include "empkg_enable.h"
#include "empkg_info.h"
#include "empkg_install.h"
#include "empkg_list.h"
#include "empkg_status.h"
#include "empkg_sync.h"

struct empkg_config config = {
	.enable = 1,
	.systemd = 1,
	.eol = 0,
	.cmd = NULL,
};

static void print_usage(const char *prog) {
	fprintf(stderr, "Usage: %s [-hpnse] <cmd>\n", prog);
	fprintf(stderr, "  -h --help|--usage     Prints this usage message\n"
	     "  -n --no-enable        do not enable apps on install\n"
	     "  -s --no-systemd       do not issue systemctl calls for daemon-reload/restart\n"
	     "  -e --set-eol          set EOL flag\n"
	     "\n"
	     "  <cmd> can be one of:\n"
	     "  list-installed        Prints the names of all installed apps, separated by newlines\n"
	     "  list-enabled          Prints the names of all enabled apps, separated by newlines\n"
	     "  list-autostart        Prints the names of all apps that autostart\n"
	     "                        their backend services, separated by newlines\n"
	     "  list-apps             Prints the names of all installed apps and their versions\n"
	     "  list                  (deprecated for list-apps)\n"
	     "\n"
	     "  status [app]          Prints the current status and manifest information of\n"
	     "                        [all/an] installed apps\n"
	     "  info <pkg>            Prints the manifest of an app package\n"
	     "\n"
	     "  install <pkg>         Installs or upgrades an app from a package file\n"
	     "  uninstall <app>       Uninstalls an app\n"
	     "  enable|disable <app>  Enables/Disables an app\n"
	     );

	exit(1);
}

static int parse_opts(int argc, char *argv[], struct empkg_config *config) {
	static const struct option lopts[] = {
		{ "help",       0, 0, 'h' },
		{ "usage",      0, 0, 'h' },
		{ "no-enable",  0, 0, 'n' },
		{ "no-systemd", 0, 0, 's' },
		{ "set-eol",    0, 0, 'e' },
		{ NULL,         0, 0,  0 },
	};
	int c;

	while (1) {
		c = getopt_long(argc, argv, "hpnse", lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'n':
			config->enable = 0;
			break;
		case 's':
			config->systemd = 0;
			break;
		case 'e':
			config->eol = 1;
			break;
		default:
			print_usage(argv[0]);
			return ERRORCODE;
		}
	}

	return 0;
}

const struct empkg_cmd empkg_cmd_list[] = {
	{ "list-installed", 0, list_installed },
	{ "list-enabled"  , 0, list_enabled   },
	{ "list-autostart", 0, list_autostart },
	{ "list-apps"     , 0, list_apps      },
	{ "list"          , 0, list_apps      }, /* deprecated */
	{ "status"        , 2, status         },
	{ "info"          , 1, info           },
	{ "install"       , 1, app_install    },
	{ "uninstall"     , 1, app_uninstall  },
	{ "enable"        , 1, app_enable     },
	{ "disable"       , 1, app_disable    },
	{ "sync"          , 0, app_sync       },
};

static int empkg_match_fn(char *argv[], struct empkg_config *config) {
	const int ncmd = ARRAY_SIZE(empkg_cmd_list);
	int a = 1, c = 0;

	while (argv[a]) {
		for(c = 0; c < ncmd; c++) {
			if (0 == (strcmp(argv[a], empkg_cmd_list[c].cmd))) {
				/* CMD match! */
				config->cmd = empkg_cmd_list[c].fn;

				/* CMD requires argument? */
				if(empkg_cmd_list[c].has_arg) {
					a++;
					if (argv[a]) {
						config->arg = strdup(argv[a]);
					} else if (1 == empkg_cmd_list[c].has_arg) {
						fprintf(stderr, "Command %s requires argument.\n", empkg_cmd_list[c].cmd);
						config->cmd = NULL;
						return ERRORCODE;
					}
				}

				return 0;
			}
		}
		a++;
	}

	return ERRORCODE;
}

/* Environment variable
 *   EMPKG_APPDIR - point to app storage dir, default /apps
 */
int main(int argc, char *argv[]) {
	char *prog_name = argv[0];
	int err;

	if (0 == strcmp(basename(argv[0]), "em-app-generator"))
		return em_app_generator(argc, argv);

	if (argc > 6) {
		fprintf(stderr, "Too many arguments\n");
		return ERRORCODE;
	}

	if (parse_opts(argc, argv, &config))
		return ERRORCODE;

	if (empkg_match_fn(argv, &config)) {
		fprintf(stderr, "Command error.\n");
		print_usage(prog_name);
		return ERRORCODE;
	}

	if (!config.cmd)
		return ERRORCODE;

	err = appdb_scan_installed();
	err |= appdb_scan_builtin();
	err |= appdb_scan_enabled();
	if (err) {
		fprintf(stderr, "Error initializing app database: %d\n", err);
		return err;
	}

	return config.cmd(config.arg);
}
