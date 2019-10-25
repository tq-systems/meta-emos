DESCRIPTION="Energy Manager App Package Manager"
SECTION="tools"

LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

SRC_URI = " \
	file://LICENSE \
	file://empkg \
	file://em-app-before.target \
	file://em-app.target \
	file://empkg.conf \
	file://src/em-app-generator.c;localdir=src \
	file://src/Makefile;localdir=src \
"

S = "${WORKDIR}/src"

DEPENDS = "jansson"
RDEPENDS_${PN} += "jq"

do_install() {
	install -d ${D}${sbindir}
	install -m 0755 ${WORKDIR}/empkg ${D}${sbindir}

	install -d ${D}${systemd_system_unitdir}
	install -m 0644 ${WORKDIR}/em-app-before.target ${WORKDIR}/em-app.target ${D}${systemd_system_unitdir}/

	install -d ${D}${datadir}/dbus-1/system.d
	install -m 0644 ${WORKDIR}/empkg.conf ${D}${datadir}/dbus-1/system.d/

	install -d ${D}${systemd_unitdir}/system-generators
	install -m 0755 ${S}/em-app-generator ${D}${systemd_unitdir}/system-generators/
}

FILES_${PN} += "\
	${systemd_system_unitdir}/em-app-before.target \
	${systemd_system_unitdir}/em-app.target \
	${systemd_unitdir}/system-generators/em-app-generator \
	${datadir}/dbus-1/system.d/empkg.conf \
"
