DESCRIPTION="Energy Manager configuration handler for em-timesync service"

LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

inherit systemd

SRC_URI = " \
	file://LICENSE \
	file://em-timesync \
	file://em-timesync-timer \
	file://em-timesync-timer.service \
"

SYSTEMD_SERVICE:${PN} = " \
	em-timesync-timer.service \
"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${base_sbindir}
	install -m 0755 \
		em-timesync \
		em-timesync-timer \
		${D}${base_sbindir}/

	install -d ${D}${sysconfdir}/systemd
	ln -s /run/em/etc/systemd/timesyncd.conf ${D}${sysconfdir}/systemd/

	install -d ${D}${systemd_unitdir}/system
	install -m 0644 \
		em-timesync-timer.service \
		${D}${systemd_unitdir}/system/
}

RDEPENDS:${PN} += "emcfg"
