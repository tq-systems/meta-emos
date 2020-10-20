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
		e2fsprogs-tune2fs \
		less \
		haveged \
		eol-led \
		status-led \
		netdev-led \
		em-power-handler \
		emos-upgrade \
		emcfg \
		empkg \
		em-flash-read \
		tzdata tzdata-americas tzdata-asia \
		tzdata-europe tzdata-africa tzdata-antarctica \
		tzdata-arctic tzdata-atlantic tzdata-australia \
		tzdata-pacific tzdata-posix \
		libstdc++ \
		avahi-daemon \
		libavahi-client \
		libdaemon \
		libmosquitto1 \
		protobuf \
		protobuf-c \
		jansson \
		gupnp \
		libsqlite3 \
		libmodbus \
		libevdev \
		libdeviceinfo \
		mosquitto \
		nginx \
		teridiand-config \
		"

IMAGE_INSTALL_append_em310 = " micrel-netdev-daemon"

IMAGE_INSTALL_append_em310 += " u-boot-fslc"
IMAGE_INSTALL_append_em300 += " u-boot-fslc"

BAD_RECOMMENDATIONS += " \
                      busybox-syslog \
                      busybox-udhcpc \
                      udev-hwdb \
                      "

IMAGE_LINGUAS = ""
IMAGE_FSTYPES = "tar"


dropbear_rsakey_dir_hook () {
	if [ -d ${IMAGE_ROOTFS}/etc/dropbear ]; then
		sed -i '/^DROPBEAR_RSAKEY_DIR=/d' ${IMAGE_ROOTFS}/etc/default/dropbear
		echo "DROPBEAR_RSAKEY_DIR=/cfglog/var/lib/dropbear" >> ${IMAGE_ROOTFS}/etc/default/dropbear
	fi
}
ROOTFS_POSTPROCESS_COMMAND += "dropbear_rsakey_dir_hook; "
IMAGE_FEATURES += "read-only-rootfs allow-root-login"
SYSTEMD_DEFAULT_TARGET = "emos.target"

COPY_LIC_MANIFEST = "1"
COPY_LIC_DIRS = "1"
