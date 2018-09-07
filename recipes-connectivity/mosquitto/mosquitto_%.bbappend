FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI += " \
	file://mosquitto.conf \
"

LICENSE = "EDL-1.0"
LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=62ddc846179e908dc0c8efec4a42ef20 \
		file://edl-v10;md5=c09f121939f063aeb5235972be8c722c \
		file://notice.html;md5=a00d6f9ab542be7babc2d8b80d5d2a4c \
"


do_install_append() {
	install -d			${D}${sysconfdir}
	install -m 0644 mosquitto.conf	${D}${sysconfdir}/mosquitto/mosquitto.conf
}
