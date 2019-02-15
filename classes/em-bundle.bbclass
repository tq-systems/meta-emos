# em-bundle.bbclass: Energy Manager upgrade bundle definitions
#
# This bundle definition includes support for rootfs and
# bootloader upgrades. The rootfs image name is determined
# automatically be replacing the first occurence of the string
# -bundle- with -image- in the recipe name. If your image names
# do not follow this scheme, RAUC_SLOT_rootfs must be set
# manually.


HOOK_SH() {
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
}


python do_unpack() {
    hookdata = d.getVar('HOOK_SH', False)
    hookfile = d.expand('${WORKDIR}/hook.sh')

    with open(hookfile, 'w') as f:
        f.write(hookdata)
}


inherit bundle

LICENSE = "gateware"
LIC_FILES_CHKSUM = "file://hook.sh;beginline=3;endline=3;md5=5592c71186793d990aedc5988d4e19c2"

RDEPENDS_${PN} += "imx28-blupdate"

RAUC_BUNDLE_COMPATIBLE = "${MACHINE}/sdk"
RAUC_BUNDLE_SLOTS = "rootfs u-boot"
RAUC_BUNDLE_HOOKS[file] = "hook.sh"

RAUC_SLOT_rootfs ?= "${@'${PN}'.replace('-bundle-', '-image-', 1)}"

RAUC_SLOT_u-boot = "u-boot"
RAUC_SLOT_u-boot[fstype] = "sb"
RAUC_SLOT_u-boot[hooks] = "install"

RAUC_KEY_FILE ?= "${@bb.utils.which(d.getVar('BBPATH'), 'files/emos/rauc/key.pem')}"
RAUC_CERT_FILE ?= "${@bb.utils.which(d.getVar('BBPATH'), 'files/emos/rauc/ca.cert.pem')}"
