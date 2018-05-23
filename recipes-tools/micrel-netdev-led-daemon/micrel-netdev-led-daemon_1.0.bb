DESCRIPTION="Updates LED netdev trigger mode according to link status of Micrel KSZ8863 switch."
SECTION="tools"

LICENSE="gateware"
LIC_FILES_CHKSUM = "file://micrel-netdev-led-daemon;beginline=3;endline=3;md5=b8ed36f855bdb9284d4d93db8c6ea536"

RDEPENDS_${PN} = "micrel-switch-tool gpio-wait"

PR = "r3"

SRC_URI = " \
	file://micrel-netdev-led-daemon \
	file://micrel-netdev-led-daemon.service \
"

inherit systemd

SYSTEMD_SERVICE_${PN} = "micrel-netdev-led-daemon.service"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${bindir}
	install -d ${D}${systemd_unitdir}/system

	install -m 0755 micrel-netdev-led-daemon			${D}${bindir}
	install -m 0644 ${WORKDIR}/micrel-netdev-led-daemon.service	${D}${systemd_unitdir}/system
}

COMPATIBLE_MACHINE = "(em300|em310)"
