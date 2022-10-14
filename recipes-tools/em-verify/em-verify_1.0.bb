DESCRIPTION="Energy Manager Verify Tool"
SECTION="tools"

LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

SRC_URI = " \
	file://LICENSE \
	file://em-verify \
"

S = "${WORKDIR}/src"

RDEPENDS:${PN} += "openssl"

do_install() {
	install -d ${D}${sbindir}
	install -m 0755 ${WORKDIR}/em-verify ${D}${sbindir}
}
