SUMMARY = "The Energy Manager Image Tool"
RDEPENDS:${PN} = "python3 python3-pyyaml rauc squashfs-tools"
RPROVIDES:${PN} += "virtual-rauc-conf"

LICENSE = "TQSPSLA-1.0.3"
LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSPSLA-1.0.3')};md5=675d9988badfa6f03ad1d2678a0d50b3"
SRC_DISTRIBUTE_LICENSES += "TQSPSLA-1.0.3"

SRC_URI = "\
	file://emit \
	file://hook.sh \
	file://ramsize.sh \
	file://bootloader-em310.sh \
	file://bootloader-em-aarch64.sh \
	file://bootloader-imx8mn-egw.sh \
"

do_install() {
	install -m755 -d ${D}${bindir} ${D}${datadir}/emit

	install -m755 ${WORKDIR}/emit ${D}${bindir}/
	install -m755 ${WORKDIR}/ramsize.sh ${D}${datadir}/emit/
	install -m644 ${WORKDIR}/hook.sh ${D}${datadir}/emit/
	for machine in em310 em-aarch64 imx8mn-egw; do
		install -m644 ${WORKDIR}/bootloader-$machine.sh ${D}${datadir}/emit/
	done
}

BBCLASSEXTEND = "native nativesdk"
