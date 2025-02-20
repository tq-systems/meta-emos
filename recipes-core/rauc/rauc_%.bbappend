FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += " \
	file://verified_message_as_gdebug.patch \
	file://10-rauc-emos.conf \
"

FILES_${PN} += "/etc/systemd/system/rauc.service.d"

do_install:append() {
	install -D -m0644 ${WORKDIR}/10-rauc-emos.conf -t ${D}${sysconfdir}/systemd/system/rauc.service.d/
}
