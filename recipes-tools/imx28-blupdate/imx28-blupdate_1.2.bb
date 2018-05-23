DESCRIPTION = "Manage i.MX28 Bootloader Updates"
SECTION = "tools"

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

DEPENDS = "libgcrypt"
RDEPENDS_${PN} = "libgcrypt"

PR = "r1"

do_install() {
	install -d ${D}${bindir}
	install -m 0755 imx28-blupdate ${D}${bindir}/imx28-blupdate
}

### Patch series -- DO NOT EDIT BELOW THIS LINE!
S = "${WORKDIR}/imx28-blupdate-v1.2"
SRC_URI += "file://imx28-blupdate-v1.2.tar.bz2"
