DESCRIPTION="Updates LED netdev trigger mode according to link status of Micrel KSZ8863 switch."
SECTION="tools"

LICENSE = "TQSSLA_V1.0.1"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=52c97447b5d8ae219abdddeb738b3140"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.1"

RDEPENDS_${PN} = "micrel-switch-tool gpio-wait"

PR = "r3"

SRC_URI = " \
	file://micrel-netdev-led-daemon \
	file://micrel-netdev-led-daemon.service \
	file://LICENSE \
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
