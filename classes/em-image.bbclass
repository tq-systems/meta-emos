# em-image.bbclass: rootfs defaults for Energy Manager
#
# This includes core system components, a number of basic
# services that should always be available, and apps that
# are preinstalled by default
#
# Recipes inheriting this class can extend IMAGE_INSTALL
# to add additional packages, or remove packages by adding
# them to IMAGE_INSTALL_remove


inherit core-image

# rootfs with 264 MiB
IMAGE_ROOTFS_SIZE_em300 = "270336"
IMAGE_ROOTFS_SIZE_em310 = "270336"


# Basic system components
IMAGE_INSTALL += " \
		packagegroup-core-boot \
		${CORE_IMAGE_EXTRA_INSTALL} \
		kernel \
		kernel-devicetree \
		kernel-modules \
		imx28-blupdate \
		u-boot-fslc-fw-utils \
		nvram-wrapper \
		mmc-utils \
		e2fsprogs \
		less \
		haveged \
		status-led \
		emos-upgrade \
		empkg \
		"

IMAGE_INSTALL_append_em300 += " netdev-led"

IMAGE_INSTALL_append_em310 += " micrel-switch-tool"
IMAGE_INSTALL_append_em310 += " micrel-netdev-led-daemon"


# System services and app container; some will become apps eventually
IMAGE_INSTALL += " \
		mosquitto \
		nginx \
		teridiand \
		web-login \
		data-transfer \
		sensor-measurement \
		web-frontend \
		"

# Preinstalled apps
IMAGE_INSTALL += " \
		em-app-device-settings \
		em-app-smart-meter \
		em-app-sensors \
		"

BAD_RECOMMENDATIONS += " \
                      busybox-syslog \
                      busybox-udhcpc \
                      udev-hwdb \
                      "

IMAGE_LINGUAS = ""


dropbear_rsakey_dir_hook () {
	if [ -d ${IMAGE_ROOTFS}/etc/dropbear ]; then
		sed -i '/^DROPBEAR_RSAKEY_DIR=/d' ${IMAGE_ROOTFS}/etc/default/dropbear
		echo "DROPBEAR_RSAKEY_DIR=/data/var/lib/dropbear" >> ${IMAGE_ROOTFS}/etc/default/dropbear
	fi
}
ROOTFS_POSTPROCESS_COMMAND += "dropbear_rsakey_dir_hook; "
IMAGE_FEATURES += "read-only-rootfs"


EXTRA_IMAGEDEPENDS += "u-boot"

COPY_LIC_MANIFEST = "1"
COPY_LIC_DIRS = "1"