# em-image.bbclass: rootfs defaults for Energy Manager
#
# This includes core system components, a number of basic
# services that should always be available, and apps that
# are preinstalled by default
#
# Recipes inheriting this class can extend IMAGE_INSTALL
# to add additional packages, or remove packages by adding
# them to IMAGE_INSTALL:remove


inherit core-image
inherit em-license-clearing

# Basic system components
IMAGE_INSTALL += " \
		packagegroup-core-boot \
		${CORE_IMAGE_EXTRA_INSTALL} \
		kernel-image \
		kernel-devicetree \
		kernel-modules \
		libubootenv-bin \
		mmc-utils \
		e2fsprogs \
		e2fsprogs-tune2fs \
		coreutils \
		util-linux-lsblk \
		util-linux-blkdiscard \
		less \
		haveged \
		emos-upgrade \
		emcfg \
		empkg \
		em-flash-read \
		em-firewall \
		em-verify \
		iproute2 \
		tzdata tzdata-americas tzdata-asia \
		tzdata-europe tzdata-africa tzdata-antarctica \
		tzdata-arctic tzdata-atlantic tzdata-australia \
		tzdata-pacific tzdata-posix \
		libstdc++ \
		avahi-daemon \
		libavahi-client \
		libdaemon \
		libmosquitto1 \
		paho-mqtt-c \
		protobuf \
		protobuf-c \
		jansson \
		gupnp \
		libsqlite3 \
		libmodbus \
		libevdev \
		libdeviceinfo \
		mosquitto \
		nftables \
		nginx \
		ca-certificates \
		glibc-utils \
		libnss-systemd \
		sudo \
		sshguard \
		"

# No new features should be added to the image for the legacy em310 hardware
IMAGE_INSTALL:remove:em310 = " \
		em-firewall \
		sshguard \
		"

BAD_RECOMMENDATIONS += " \
		busybox-syslog \
		busybox-udhcpc \
		udev-hwdb \
		"

PACKAGE_EXCLUDE:task-rootfs += "python*"

IMAGE_LINGUAS = ""
IMAGE_FSTYPES = "tar"

IMAGE_NAME_SUFFIX = ""
IMAGE_VERSION_SUFFIX = "-${DISTRO_VERSION}.rootfs"

dropbear_sshkey_dir_hook () {
	if [ -d ${IMAGE_ROOTFS}/etc/dropbear ]; then
		sed -i '/^DROPBEAR_SSHKEY_DIR=/d' ${IMAGE_ROOTFS}/etc/default/dropbear
		echo "DROPBEAR_SSHKEY_DIR=/cfglog/var/lib/dropbear" >> ${IMAGE_ROOTFS}/etc/default/dropbear
	fi
}

os_machine_hook () {
	echo "${MACHINE}" > ${IMAGE_ROOTFS}/etc/os-machine
}

ROOTFS_POSTPROCESS_COMMAND += "dropbear_sshkey_dir_hook; os_machine_hook;"

IMAGE_FEATURES += "read-only-rootfs allow-root-login"
SYSTEMD_DEFAULT_TARGET = "emos.target"

COPY_LIC_MANIFEST = "1"
COPY_LIC_DIRS = "1"
