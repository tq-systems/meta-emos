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

PRODUCT_INFO_PACKAGE ?= "em-product-info-${IMAGE_VARIANT}"

# rootfs with 384 MiB (raw disk space)
IMAGE_ROOTFS_SIZE_em300 = "393216"
IMAGE_ROOTFS_SIZE_em310 = "393216"


# Basic system components
IMAGE_INSTALL += " \
		packagegroup-core-boot \
		${CORE_IMAGE_EXTRA_INSTALL} \
		kernel \
		kernel-devicetree \
		kernel-modules \
		imx28-blupdate \
		u-boot-fslc-fw-utils \
		mmc-utils \
		e2fsprogs \
		less \
		haveged \
		status-led \
		netdev-led \
		emos-upgrade \
		emcfg \
		empkg \
		button-handler \
		tzdata tzdata-americas tzdata-asia \
		tzdata-europe tzdata-africa tzdata-antarctica \
		tzdata-arctic tzdata-atlantic tzdata-australia \
		tzdata-pacific tzdata-posix \
		${PRODUCT_INFO_PACKAGE} \
		"

IMAGE_INSTALL_append_em310 += " micrel-netdev-daemon"

IMAGE_INSTALL_append_em300 += " flash-scan"
IMAGE_INSTALL_append_em310 += " flash-scan"

# System services; some will become apps eventually
IMAGE_INSTALL += " \
		mosquitto \
		nginx \
		teridiand \
		sensor-measurement \
		"

# Preinstalled apps
IMAGE_INSTALL += " \
		em-app-web-application \
		em-app-data-transfer \
		em-app-device-settings \
		em-app-smart-meter \
		em-app-upnp \
		em-app-web-login \
		"

# Install u-boot to rootfs only for shipping license information
IMAGE_INSTALL_append_em310 += " u-boot-fslc"
IMAGE_INSTALL_append_em300 += " u-boot-fslc"

BAD_RECOMMENDATIONS += " \
                      busybox-syslog \
                      busybox-udhcpc \
                      udev-hwdb \
                      "

IMAGE_LINGUAS = ""


dropbear_rsakey_dir_hook () {
	if [ -d ${IMAGE_ROOTFS}/etc/dropbear ]; then
		sed -i '/^DROPBEAR_RSAKEY_DIR=/d' ${IMAGE_ROOTFS}/etc/default/dropbear
		echo "DROPBEAR_RSAKEY_DIR=/cfglog/var/lib/dropbear" >> ${IMAGE_ROOTFS}/etc/default/dropbear
	fi
}
ROOTFS_POSTPROCESS_COMMAND += "dropbear_rsakey_dir_hook; "
IMAGE_FEATURES += "read-only-rootfs allow-root-login"

COPY_LIC_MANIFEST = "1"
COPY_LIC_DIRS = "1"
