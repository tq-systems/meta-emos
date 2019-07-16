SUMMARY = "The Energy Manager Image Tool"
RDEPENDS_${PN} = "python3 python3-pyyaml rauc squashfs-tools"

LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/emit;beginline=2;endline=5;md5=0628c1a390d66a2f452e92e03fef067f"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"


SRC_URI = "\
	file://emit \
	file://hook.sh \
	file://base.yml \
"

do_install() {
	install -m755 -d ${D}${bindir} ${D}${datadir}/emit

	install -m755 ${WORKDIR}/emit ${D}${bindir}/
	install -m644 ${WORKDIR}/hook.sh ${D}${datadir}/emit/
	install -m644 ${WORKDIR}/base.yml ${D}${datadir}/emit/
}

BBCLASSEXTEND = "native nativesdk"
