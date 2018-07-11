#!/bin/sh
#
# (c) Gateware Communications GmbH, Nuremberg 2018
#

set -eo pipefail

echo "i.MX28 bootloader install script"

if [ "$1" != "slot-install" ]; then
	echo "Invalid action '$1'! Aborting." >&2
	exit 1
fi
if [ "$RAUC_SLOT_CLASS" != "u-boot" ]; then
	echo "Invalid slot class '$RAUC_SLOT_CLASS'! Aborting" >&2
	exit 1
fi

BOOT_SLOT="$(imx28-blupdate status --device "$RAUC_SLOT_DEVICE" --boot-source | awk '/NOTICE: Boot source: / {print $4}')"

case "$BOOT_SLOT" in
Primary)
	SLOTS='second first'
	;;
Secondary)
	SLOTS='first second'
	;;
*)
	echo "Unable to determine boot slot! Aborting." >&2
	exit 1
esac

for slot in $SLOTS; do
	INSTALLED=$( \
		imx28-blupdate extract --device "$RAUC_SLOT_DEVICE" --$slot - \
		| sha256sum | awk '{print $1}' || true \
	)
	if [ "$INSTALLED" == "$RAUC_IMAGE_DIGEST" ]; then
		echo "Bootloader in $slot slot is already up to date! Skipping."
	else
		imx28-blupdate upgrade --device "$RAUC_SLOT_DEVICE" --$slot "$RAUC_IMAGE_NAME"
		imx28-blupdate status --device "$RAUC_SLOT_DEVICE" --$slot
	fi
done
