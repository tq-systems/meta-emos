DESCRIPTION="Energy Manager Verify Tool"
SECTION="tools"

LICENSE = "TQSPSLA-1.0.3"
LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSPSLA-1.0.3')};md5=675d9988badfa6f03ad1d2678a0d50b3"
SRC_DISTRIBUTE_LICENSES += "TQSPSLA-1.0.3"

SRC_URI = " \
	file://em-verify \
"

S = "${WORKDIR}/src"

RDEPENDS:${PN} += "openssl"

do_install() {
	install -d ${D}${sbindir}
	install -m 0755 ${WORKDIR}/em-verify ${D}${sbindir}
}
