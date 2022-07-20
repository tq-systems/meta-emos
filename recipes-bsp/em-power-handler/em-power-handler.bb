LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

inherit systemd

SRC_URI = " \
	file://LICENSE \
	file://em-power-handler.service \
	file://em-power-handler \
"

SYSTEMD_SERVICE_${PN} = "em-power-handler.service"
RDEPENDS_${PN} = "libgpiod-tools"

do_install () {
	install -d ${D}${base_sbindir}
	install -m 755 ${WORKDIR}/em-power-handler ${D}${base_sbindir}/

	install -d ${D}${systemd_system_unitdir}
	install -m 644 ${WORKDIR}/em-power-handler.service ${D}${systemd_system_unitdir}/
}

COMPATIBLE_MACHINE = "em"
