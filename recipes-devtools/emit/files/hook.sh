#!/bin/sh

#
# Copyright (C) 2020, TQ-Systems GmbH
#
# slot-install handling is:
#
# (c) Gateware Communications GmbH, Nuremberg 2018
#

set -eo pipefail


BASEDIR="$(dirname "$0")"

. "$BASEDIR"/bootloader.sh


check_version() {
	if [ -e "$BASEDIR"/allow-downgrade ]; then
		return 0
	fi

	local system_version="$1" bundle_version="$2"

	# Get prefix composed of digits and periods, we don't care about snapshot suffixes etc.
	system_version="$(echo "$system_version" | grep -Eo '^[0-9.]+')"
	bundle_version="$(echo "$bundle_version" | grep -Eo '^[0-9.]+')"

	# Take the first 3 fields (separated by periods), sort each numerically
	# Sub-patchlevel numbers are ignored if they exist
	local lower_version="$(
		printf '%s\n%s\n' "$system_version" "$bundle_version" | \
		sort -s -t . -k 1,1n -k 2,2n -k 3,3n | \
		head -n 1
	)"

	# Return true when the system version is the lower on of the two versions
	# - so this is an upgrade, or both version strings are equal
	if [ "$system_version" = "$lower_version" ]; then
		return 0
	else
		return 1
	fi
}


case "$1" in

install-check)
	# Split compatible strings at /
	oldIFS="$IFS"; IFS='/'

	set -- $RAUC_SYSTEM_COMPATIBLE
	SYSTEM_MACHINE="$1"
	SYSTEM_COMPATIBLE="$2"
	SYSTEM_FORMAT="$3"
	SYSTEM_VERSION="$4"

	set -- $RAUC_MF_COMPATIBLE
	MF_MACHINE="$1"
	MF_COMPATIBLE="$2"

	IFS="$oldIFS"


	if [ "$MF_MACHINE" != "$SYSTEM_MACHINE" ]; then
		echo "Your hardware is not supported by this firmware bundle." >&2
		exit 10
	fi

	if [ -e /run/ignore-compatible ]; then
		exit 0
	fi

	if [ "$MF_COMPATIBLE" != "$SYSTEM_COMPATIBLE" ]; then
		echo "Incorrect firmware." >&2
		exit 10
	fi

	case "$SYSTEM_FORMAT" in
	'')
		# Old firmware, upgrade is always allowed
		;;

	'1')
		if ! check_version "$SYSTEM_VERSION" "$RAUC_MF_VERSION"; then
			echo "Installed firmware too new, downgrades are not supported." >&2
			exit 10
		fi
		;;

	*)
		# Unknown compatible string format, the installed firmware
		# must be newer than this script
		echo "Installed firmware too new, downgrades are not supported." >&2
		exit 10
		;;
	esac

	;;

slot-install)
	if [ "$RAUC_SLOT_CLASS" != "u-boot" ]; then
		echo "Invalid slot class '$RAUC_SLOT_CLASS'! Aborting" >&2
		exit 1
	fi

	for slot in $(bootloader_slots); do
		bootloader_upgrade "$slot"
	done

	;;

*)
	exit 1
	;;

esac

exit 0
