DESCRIPTION="Energy Manager App Package Manager"
SECTION="tools"

LICENSE = "TQSSLA_V1.0.1"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=52c97447b5d8ae219abdddeb738b3140"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.1"

SRC_URI = " \
	file://LICENSE \
	file://empkg \
	file://em-app-generator \
	file://empkg.conf \
"

S = "${WORKDIR}"

RDEPENDS_${PN} += "jq"

do_install() {
	install -d ${D}${sbindir}
	install -m 0755 empkg ${D}${sbindir}

	install -d ${D}${systemd_unitdir}/system-generators
	install -m 0755 em-app-generator ${D}${systemd_unitdir}/system-generators/

	install -d ${D}${datadir}/dbus-1/system.d
	install -m 0644 empkg.conf ${D}${datadir}/dbus-1/system.d/
}

FILES_${PN} += "${systemd_unitdir}/system-generators/em-app-generator ${datadir}/dbus-1/system.d/empkg.conf"
