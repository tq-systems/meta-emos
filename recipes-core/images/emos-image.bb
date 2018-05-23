inherit core-image

# rootfs with 264 MiB
IMAGE_ROOTFS_SIZE_em310 = "270336"

IMAGE_INSTALL += " packagegroup-core-boot ${CORE_IMAGE_EXTRA_INSTALL}"

IMAGE_INSTALL += " \
		kernel \
		kernel-devicetree \
		kernel-modules \
		imx28-blupdate \
		emos-upgrade \
		u-boot-fslc-fw-utils \
		mmc-utils \
		e2fsprogs \
		less \
		teridiand \
		mosquitto \
		nginx \
		nvram-wrapper \
		"

# go applications, probably moving to appfs later
IMAGE_INSTALL += " \
		data-transfer \
		device-config \
		sensor-measurement \
		submeter \
		web-frontend \
		web-login \
		"

BAD_RECOMMENDATIONS += " \
                      busybox-syslog \
                      busybox-udhcpc \
                      "

# for em310 only
IMAGE_INSTALL_append_em310 += " micrel-switch-tool"
IMAGE_INSTALL_append_em310 += " micrel-netdev-led-daemon"

IMAGE_LINGUAS = ""

EXTRA_IMAGEDEPENDS += "u-boot"
