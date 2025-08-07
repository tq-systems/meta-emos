LICENSE = "TQSPSLA-1.0.3"
LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSPSLA-1.0.3')};md5=675d9988badfa6f03ad1d2678a0d50b3"
SRC_DISTRIBUTE_LICENSES += "TQSPSLA-1.0.3"

DESCRIPTION = "Teridian metering processor querying daemon - hardware-specific configuration"

SRC_URI = " \
	file://teridiand_hw.conf \
	file://teridian_symlink \
	file://70-teridian.rules \
"

RDEPENDS:${PN} = "libgpiod-cli"

do_install () {
	install -d ${D}${sysconfdir}
	install -m 0644 ${WORKDIR}/teridiand_hw.conf ${D}${sysconfdir}/teridiand_hw.conf

	install -d ${D}${base_libdir}/udev/rules.d/
	install -m 0755 ${WORKDIR}/teridian_symlink ${D}${base_libdir}/udev/
	install -m 0644 ${WORKDIR}/70-teridian.rules ${D}${base_libdir}/udev/rules.d/
}
