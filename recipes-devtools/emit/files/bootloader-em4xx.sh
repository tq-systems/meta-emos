#
# Copyright (C) 2020, TQ-Systems GmbH
#
# Author: Matthias Schiffer <matthias.schiffer@tq-group.com>
#


MMCBLK=mmcblk0

bootloader_slots () {
        local part_config=$(mmc extcsd read "/dev/${MMCBLK}" | \
                sed -nr '/\[PARTITION_CONFIG: .+\]/{s/^.*\[PARTITION_CONFIG: (.+)\]$/\1/;p}')

	case "$(((part_config >> 3) & 0x7))" in
	1)
		echo '2 1'
		;;
	2)
		echo '1 2'
		;;
	*)
		echo "Warning: unrecognized PARTITION_CONFIG: ${part_config}" >&2
		echo '1 2'
	esac
}

bootloader_upgrade () {
	local slot="$1"

	local dev="${MMCBLK}boot$((slot - 1))"
	local size="$(lsblk -bnro size "/dev/${dev}")"

	if (
		dd if="$RAUC_IMAGE_NAME" bs="$size" count=1 conv=sync 2>/dev/null | \
			cmp -s "/dev/${dev}"
	); then
		echo "Bootloader in slot $slot is already up to date! Skipping."
	else
		echo "Updating bootloader in slot $slot"

		echo 0 >"/sys/class/block/${dev}/force_ro"
		dd if="$RAUC_IMAGE_NAME" of="/dev/${dev}" bs="$size" count=1 conv=sync 2>/dev/null
		sync "/dev/${dev}"
		echo 1 >"/sys/class/block/${dev}/force_ro"
	fi

	mmc bootpart enable "${slot}" 0 "/dev/${MMCBLK}"
}
