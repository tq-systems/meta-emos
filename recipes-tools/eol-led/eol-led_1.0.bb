DESCRIPTION="eol-led is setting all LEDs for end of life mode."
SECTION="tools"

LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

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

	install -m 0755 eol-led			${D}${bindir}
	install -m 0644 ${WORKDIR}/eol-led.service	${D}${systemd_unitdir}/system
}

COMPATIBLE_MACHINE = "(em300|em310)"