LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

DESCRIPTION = "Tool for reading EXTCSD registers of eMMC"

SRC_URI = " \
	file://LICENSE \
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
