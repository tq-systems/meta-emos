DESCRIPTION="Energy Manager configuration handler for em-timesync service"

LICENSE = "TQSPSLA-1.0.3"
LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSPSLA-1.0.3')};md5=675d9988badfa6f03ad1d2678a0d50b3"
SRC_DISTRIBUTE_LICENSES += "TQSPSLA-1.0.3"

inherit systemd

SRC_URI = " \
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
