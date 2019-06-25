DESCRIPTION="Updates LED netdev trigger mode according to link status of Micrel KSZ8863 switch."
SECTION="tools"

LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

RDEPENDS_${PN} = "em-app-micrel-switch-tool libgpiod-tools"

PR = "r3"

SRC_URI = " \
	file://micrel-netdev-daemon \
	file://micrel-netdev-daemon.service \
	file://LICENSE \
"

inherit systemd

SYSTEMD_SERVICE_${PN} = "micrel-netdev-daemon.service"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${bindir}
	install -d ${D}${systemd_unitdir}/system

	install -m 0755 micrel-netdev-daemon			${D}${bindir}
	install -m 0644 ${WORKDIR}/micrel-netdev-daemon.service	${D}${systemd_unitdir}/system
}

COMPATIBLE_MACHINE = "em310"
