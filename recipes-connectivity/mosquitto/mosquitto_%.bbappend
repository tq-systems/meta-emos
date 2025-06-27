FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append:emos = " \
	file://mosquitto.conf \
	file://mosquitto.service \
"

LICENSE = "EDL-1.0"
LIC_FILES_CHKSUM = " \
	file://LICENSE.txt;md5=ca9a8f366c6babf593e374d0d7d58749 \
	file://edl-v10;md5=9f6accb1afcb570f8be65039e2fcd49e \
	file://NOTICE.md;md5=a7a91b4754c6f7995020d1b49bc829c6 \
"

do_install:append:emos() {
	install -d ${D}${sysconfdir}
	install -m 0644 ${WORKDIR}/mosquitto.conf		${D}${sysconfdir}/mosquitto/
	install -m 0644 ${WORKDIR}/mosquitto.service	${D}${systemd_unitdir}/system/
}
BBCLASSEXTEND += "nativesdk"
