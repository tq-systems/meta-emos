DESCRIPTION="Energy Manager App Package Manager"
SECTION="tools"

LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

SRC_URI = " \
	file://LICENSE \
	file://empkg \
	file://em-app.target \
	file://em-app-generator \
	file://empkg.conf \
"

S = "${WORKDIR}"

RDEPENDS_${PN} += "jq"

do_install() {
	install -d ${D}${sbindir}
	install -m 0755 empkg ${D}${sbindir}

	install -d ${D}${systemd_system_unitdir}
	install -m 0644 em-app.target ${D}${systemd_system_unitdir}/

	install -d ${D}${systemd_unitdir}/system-generators
	install -m 0755 em-app-generator ${D}${systemd_unitdir}/system-generators/

	install -d ${D}${datadir}/dbus-1/system.d
	install -m 0644 empkg.conf ${D}${datadir}/dbus-1/system.d/
}

FILES_${PN} += "\
	${systemd_system_unitdir}/em-app.target \
	${systemd_unitdir}/system-generators/em-app-generator \
	${datadir}/dbus-1/system.d/empkg.conf \
"
