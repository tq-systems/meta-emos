#
# Copyright (C) 2020, TQ-Systems GmbH
#
# Based on:
#
# (c) Gateware Communications GmbH, Nuremberg 2018
#


bootloader_slots () {
	local active_slot=$(
                imx28-blupdate status --device "$RAUC_SLOT_DEVICE" --boot-source \
                        | awk '/NOTICE: Boot source: / {print $4}'
        )
	case "$active_slot" in
	Primary)
		echo 'second first'
		;;
	Secondary)
		echo 'first second'
		;;
	*)
		echo "Unable to determine boot slot! Aborting." >&2
		exit 1
	esac
}

bootloader_upgrade () {
        local slot="$1"

	local installed=$( \
		imx28-blupdate extract --device "$RAUC_SLOT_DEVICE" --$slot - \
		        | sha256sum | awk '{print $1}' || true \
	)
	if [ "$installed" = "$RAUC_IMAGE_DIGEST" ]; then
		echo "Bootloader in slot $slot is already up to date! Skipping."
		return
	fi

	echo "Updating bootloader in slot $slot"

	imx28-blupdate upgrade --device "$RAUC_SLOT_DEVICE" --$slot "$RAUC_IMAGE_NAME"
	imx28-blupdate status --device "$RAUC_SLOT_DEVICE" --$slot
}
