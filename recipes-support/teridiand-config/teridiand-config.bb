LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSSLA_V1.0.2')};md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

DESCRIPTION = "Teridian metering processor querying daemon - hardware-specific configuration"

SRC_URI = " \
	file://teridiand_hw.conf \
	file://teridian_symlink \
	file://70-teridian.rules \
"

do_install () {
	install -d ${D}${sysconfdir}
	install -m 0644 ${WORKDIR}/teridiand_hw.conf ${D}${sysconfdir}/teridiand_hw.conf

	install -d ${D}${base_libdir}/udev/rules.d/
	install -m 0755 ${WORKDIR}/teridian_symlink ${D}${base_libdir}/udev/
	install -m 0644 ${WORKDIR}/70-teridian.rules ${D}${base_libdir}/udev/rules.d/
}
