DESCRIPTION="status-led is setting the status LED after boot."
SECTION="tools"

LICENSE = "TQSPSLA-1.0.3"
LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSPSLA-1.0.3')};md5=675d9988badfa6f03ad1d2678a0d50b3"

SRC_URI = " \
	file://${BPN} \
	file://${BPN}.service \
"

inherit systemd

SYSTEMD_SERVICE:${PN} = "${BPN}.service"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${bindir}
	install -d ${D}${systemd_unitdir}/system

	install -m 0755 status-led			${D}${bindir}
	install -m 0644 status-led.service	${D}${systemd_unitdir}/system
}

COMPATIBLE_MACHINE = "em"
