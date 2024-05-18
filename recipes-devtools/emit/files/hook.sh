#!/bin/sh

#
# Copyright (C) 2020-2024, TQ-Systems GmbH
#
# slot-install handling is:
#
# (c) Gateware Communications GmbH, Nuremberg 2018
#

# Do not set -e to ensure that we exit with code 10 in install-check
set -o pipefail


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

device_variant() {
	# For maximum compatibility, avoid head -z, which is not available in busybox
	# compatible: replace ',' with '_' to turn 'tq,..' into 'tq_...', which is used as bootloader filename
	local compatible="$(tr </proc/device-tree/compatible '\0' '\n' | head -n 1 | tr ',' '_' )"
	local ram_size="$("$BASEDIR"/ramsize.sh)"

	echo "${compatible}_${ram_size}m"
}

bootloader_digest() {
	local variant="$1"

	while read -r digest name; do
		if [ "$name" = "$variant" ]; then
			echo "$digest"
			return 0
		fi
	done <"$BASEDIR/bootloaders.txt"

	return 1
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

	# Backwards compat for pre-unification machine name
	if [ "$MF_MACHINE" = 'em-aarch64' ] && [ "$SYSTEM_MACHINE" = 'em4xx' ]; then
		SYSTEM_MACHINE='em-aarch64'
	fi

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

		# run the pre-update migration if it exists
		if [ -e "$BASEDIR/migration" ]; then
			# a failed migration does not cancel the update
			"$BASEDIR/migration" "$SYSTEM_VERSION" "$RAUC_MF_VERSION" "$SYSTEM_MACHINE" \
				|| echo "Pre-update migration failed" >&2
		fi
		;;

	*)
		# Unknown compatible string format, the installed firmware
		# must be newer than this script
		echo "Installed firmware too new, downgrades are not supported." >&2
		exit 10
		;;
	esac

	variant="$(device_variant)"
	if [ -z "$variant" ]; then
		echo "Unable to determine device variant" >&2
		exit 10
	fi


	if ! bootloader_digest "$variant" >/dev/null; then
		echo "No matching bootloader found for your device variant '$variant'" >&2
		exit 10
	fi

	;;

slot-install)
	set -e

	if [ "$RAUC_SLOT_CLASS" != "u-boot" ]; then
		echo "Invalid slot class '$RAUC_SLOT_CLASS'! Aborting" >&2
		exit 1
	fi

	# Override RAUC-provided variables with variant-specific values
	variant="$(device_variant)"
	RAUC_IMAGE_NAME="${BASEDIR}/bootloader-${variant}.bin"
	RAUC_IMAGE_DIGEST="$(bootloader_digest "$variant")"

	if [ -z "$variant" ] || [ -z "$RAUC_IMAGE_DIGEST" ] || [ ! -e "$RAUC_IMAGE_NAME" ]; then
		echo "Bootloader variant image not found!" >&2
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
