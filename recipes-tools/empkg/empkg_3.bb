DESCRIPTION="Energy Manager App Package Manager"
SECTION="tools"

LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

SRC_URI = " \
	file://LICENSE \
	file://em-app-before.target \
	file://em-app.target \
	file://em-app-time.target \
	file://em-app-no-time.target \
	file://empkg.conf \
	file://src;localdir=src \
"

S = "${WORKDIR}/src"

DEPENDS = "jansson libarchive openssl systemd acl"

do_install() {
	install -d ${D}${sbindir}
	install -m 0755 ${WORKDIR}/src/empkg ${D}${sbindir}

	install -d ${D}${systemd_system_unitdir}
	install -m 0644 \
		${WORKDIR}/em-app-before.target \
		${WORKDIR}/em-app.target \
		${WORKDIR}/em-app-time.target \
		${WORKDIR}/em-app-no-time.target \
		${D}${systemd_system_unitdir}/

	install -d ${D}${datadir}/dbus-1/system.d
	install -m 0644 ${WORKDIR}/empkg.conf ${D}${datadir}/dbus-1/system.d/

	install -d ${D}${systemd_unitdir}/system-generators
	ln -sr ${D}${sbindir}/empkg ${D}${systemd_unitdir}/system-generators/em-app-generator
}

FILES:${PN} += "\
	${systemd_system_unitdir}/em-app-before.target \
	${systemd_system_unitdir}/em-app.target \
	${systemd_system_unitdir}/em-app-time.target \
	${systemd_system_unitdir}/em-app-no-time.target \
	${systemd_unitdir}/system-generators/em-app-generator \
	${datadir}/dbus-1/system.d/empkg.conf \
"

