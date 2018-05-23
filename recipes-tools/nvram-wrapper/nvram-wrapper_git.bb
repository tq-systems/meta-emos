DESCRIPTION="Wrapper for u-boot-fw-utils."
SECTION="tools"

DEPENDS = "u-boot-fslc-fw-utils"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

SRC_URI = " \
	file://nvram \
	file://COPYING \
"

S = "${WORKDIR}"
do_install() {
	install -d		${D}${sbindir}
	install -m 0755 nvram	${D}${sbindir}/nvram
}
