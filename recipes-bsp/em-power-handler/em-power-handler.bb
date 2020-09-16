LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${WORKDIR}/em-power-handler;beginline=3;endline=6;md5=785f0a63d6f38dba55a52d43fef3ad61"

inherit systemd

SRC_URI = " \
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

COMPATIBLE_MACHINE = "(em300|em310|em4xx)"
