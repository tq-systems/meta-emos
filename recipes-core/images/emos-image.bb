inherit core-image

# rootfs with 264 MiB
IMAGE_ROOTFS_SIZE_em300 = "270336"
IMAGE_ROOTFS_SIZE_em310 = "270336"



IMAGE_INSTALL += " packagegroup-core-boot ${CORE_IMAGE_EXTRA_INSTALL}"

IMAGE_INSTALL += " \
		kernel \
		kernel-devicetree \
		kernel-modules \
		imx28-blupdate \
		emos-upgrade \
		empkg \
		u-boot-fslc-fw-utils \
		mmc-utils \
		e2fsprogs \
		less \
		teridiand \
		haveged \
		mosquitto \
		nginx \
		nvram-wrapper \
		status-led \
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
                      udev-hwdb \
                      "

# for em310 only
IMAGE_INSTALL_append_em310 += " micrel-switch-tool"
IMAGE_INSTALL_append_em310 += " micrel-netdev-led-daemon"

# for em300 only
IMAGE_INSTALL_append_em300 += " netdev-led"

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
