DESCRIPTION="Energy Manager App Package Manager"
SECTION="tools"

LICENSE = "TQSPSLA-1.0.3"
LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSPSLA-1.0.3')};md5=675d9988badfa6f03ad1d2678a0d50b3"
SRC_DISTRIBUTE_LICENSES += "TQSPSLA-1.0.3"

SRC_URI = " \
	file://empkg \
	file://em-app-before.target \
	file://em-app.target \
	file://em-app-time.target \
	file://em-app-no-time.target \
	file://empkg.conf \
	file://src/em-app-generator.c;localdir=src \
	file://src/Makefile;localdir=src \
"

S = "${WORKDIR}/src"

DEPENDS = "jansson"
RDEPENDS:${PN} += "jq acl"

do_install() {
	install -d ${D}${sbindir}
	install -m 0755 ${WORKDIR}/empkg ${D}${sbindir}

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
	install -m 0755 ${S}/em-app-generator ${D}${systemd_unitdir}/system-generators/
}

FILES:${PN} += "\
	${systemd_system_unitdir}/em-app-before.target \
	${systemd_system_unitdir}/em-app.target \
	${systemd_system_unitdir}/em-app-time.target \
	${systemd_system_unitdir}/em-app-no-time.target \
	${systemd_unitdir}/system-generators/em-app-generator \
	${datadir}/dbus-1/system.d/empkg.conf \
"
