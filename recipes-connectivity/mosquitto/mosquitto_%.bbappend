FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI += " \
	file://mosquitto.conf \
"

do_install_append() {
	install -d			${D}${sysconfdir}
	install -m 0644 mosquitto.conf	${D}${sysconfdir}/mosquitto/mosquitto.conf
}
