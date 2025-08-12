LICENSE = "TQSPSLA-1.0.3"
LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSPSLA-1.0.3')};md5=675d9988badfa6f03ad1d2678a0d50b3"
SRC_DISTRIBUTE_LICENSES += "TQSPSLA-1.0.3"

DESCRIPTION = "Tool for reading EXTCSD registers of eMMC"

SRC_URI = " \
	file://em-flash-read \
	file://em-flash_hw.conf \
"

S = "${WORKDIR}"

do_install () {
	install -d ${D}${bindir}
	install -m 0755 em-flash-read	${D}${bindir}

	install -d ${D}${sysconfdir}
	install -m 0644 em-flash_hw.conf	${D}${sysconfdir}/em-flash_hw.conf
}

FILES:${PN} += " \
	${sysconfdir}/em-flash_hw.conf \
	${bindir}/em-flash-read \
"
