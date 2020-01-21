# Copyright (C) 2018, TQ Systems
# Author: Matthias Schiffer <matthias.schiffer@tq-group.com>

DESCRIPTION = "EMOS RAUC wrapper"
SECTION = "core"

LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://emos-upgrade;beginline=3;endline=5;md5=81086d9e1356cd540984044d078650e8"

PR = "r1"

RDEPENDS_${PN} = "rauc"

SRC_URI = " \
        file://emos-upgrade \
        file://emos-upgrade-finalize \
        file://emos-upgrade-finalize.service \
        file://emos-upgrade-status \
"

S = "${WORKDIR}"


FILES_${PN} += "${libdir}/emos/upgrade/emos-upgrade-finalize"
SYSTEMD_SERVICE_${PN} = "emos-upgrade-finalize.service"

inherit systemd


do_install() {
        install -d ${D}${sbindir}/
        install -m 0755 emos-upgrade ${D}${sbindir}/
        install -m 0755 emos-upgrade-status ${D}${sbindir}/

        install -d ${D}${libdir}/emos/upgrade/
        install -m 0755 emos-upgrade-finalize ${D}${libdir}/emos/upgrade/

        install -d ${D}${systemd_unitdir}/system/
        install -m 0644 emos-upgrade-finalize.service ${D}${systemd_unitdir}/system/
}
