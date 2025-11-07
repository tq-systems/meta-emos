LICENSE = "TQSPSLA-1.0.3"
LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSPSLA-1.0.3')};md5=675d9988badfa6f03ad1d2678a0d50b3"

inherit systemd

SRC_URI = " \
	file://em-power-handler.service \
	file://em-power-handler \
"

SYSTEMD_SERVICE:${PN} = "em-power-handler.service"
RDEPENDS:${PN} = "libgpiod-tools"

do_install () {
	install -d ${D}${base_sbindir}
	install -m 755 ${WORKDIR}/em-power-handler ${D}${base_sbindir}/

	install -d ${D}${systemd_system_unitdir}
	install -m 644 ${WORKDIR}/em-power-handler.service ${D}${systemd_system_unitdir}/
}

COMPATIBLE_MACHINE = "em"
