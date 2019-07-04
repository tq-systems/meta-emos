LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSSLA_V1.0.2')};md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

SRC_URI = " \
	file://6545eq5_6603_REV${PV}.bin \
"

do_install () {
	install -d ${D}${datadir}/teridiand
	install -m 0644 ${WORKDIR}/6545eq5_6603_REV${PV}.bin ${D}${datadir}/teridiand/6545eq5_6603.bin
}

FILES_${PN} += "${datadir}/teridiand/6545eq5_6603.bin"
