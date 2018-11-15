DESCRIPTION = "Report faked system time to programs"

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${S}/COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

SRC_URI = "git://github.com/wolfcw/libfaketime.git;protocol=https"
# tag v0.9.7
SRCREV = "c9a681c3e36c453affb7ff393458478a953d886c"

PACKAGES += "libfaketime"
RDEPENDS_${PN} = "libfaketime"

S = "${WORKDIR}/git"

EXTRA_OEMAKE = "PREFIX=${prefix}"

do_install() {
	oe_runmake install DESTDIR=${D}
	rm ${D}${libdir}/faketime/libfaketimeMT.so.1
}

FILES_libfaketime = "${libdir}/faketime/libfaketime.so.1"
FILES_${PN} = "${bindir}/faketime"
