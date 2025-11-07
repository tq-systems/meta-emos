# Copyright (C) 2018, TQ-Systems GmbH
# Author: Matthias Schiffer <matthias.schiffer@tq-group.com>

DESCRIPTION = "EMOS RAUC wrapper"
SECTION = "core"

LICENSE = "TQSPSLA-1.0.3"
LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSPSLA-1.0.3')};md5=675d9988badfa6f03ad1d2678a0d50b3"

PR = "r1"

RDEPENDS:${PN} = "rauc"

SRC_URI = " \
	file://emos-upgrade \
	file://emos-upgrade-cleanup.service \
	file://emos-upgrade-finalize \
	file://emos-upgrade-finalize.service \
	file://emos-upgrade-status \
"

S = "${WORKDIR}"


FILES:${PN} += "${libdir}/emos/upgrade/emos-upgrade-finalize"
SYSTEMD_SERVICE:${PN} = "emos-upgrade-cleanup.service emos-upgrade-finalize.service"

inherit systemd


do_install() {
	install -d ${D}${sbindir}/
	install -m 0755 emos-upgrade ${D}${sbindir}/
	install -m 0755 emos-upgrade-status ${D}${sbindir}/

	install -d ${D}${libdir}/emos/upgrade/
	install -m 0755 emos-upgrade-finalize ${D}${libdir}/emos/upgrade/

	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 emos-upgrade-cleanup.service  ${D}${systemd_unitdir}/system/
	install -m 0644 emos-upgrade-finalize.service ${D}${systemd_unitdir}/system/
}
