DESCRIPTION="netdev-led is setting the mode of the network LED."
SECTION="tools"

LICENSE = "TQSSLA_V1.0.1"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=52c97447b5d8ae219abdddeb738b3140"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.1"

SRC_URI = " \
	file://LICENSE \
	file://${PN} \
	file://${PN}.service \
"

inherit systemd

SYSTEMD_SERVICE_${PN} = "${PN}.service"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${bindir}
	install -d ${D}${systemd_unitdir}/system

	install -m 0755 netdev-led			${D}${bindir}
	install -m 0644 ${WORKDIR}/netdev-led.service	${D}${systemd_unitdir}/system
}

COMPATIBLE_MACHINE = "(em300|em310)"
