DESCRIPTION = "Manage i.MX28 Bootloader Updates"
SECTION = "tools"

SRC_URI = "https://packages.tq-group.com/energymanager/imx28-blupdate-v${PV}.tar.bz2"
SRC_URI[sha256sum] = "1ae47619efd3b45272bfb7d65b17f0748d2b2e98a86c0994122d71576128e235"

S = "${WORKDIR}/imx28-blupdate-v${PV}"

LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

DEPENDS = "libgcrypt"

PR = "r1"

CFLAGS:append = "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"

do_install() {
	install -d ${D}${bindir}
	install -m 0755 imx28-blupdate ${D}${bindir}/imx28-blupdate
}
