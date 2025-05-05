DESCRIPTION = "Energy Manager Firewall"

LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

SRC_URI = " \
          file://00-em-firewall-init.conf \
          file://em-firewall \
          file://em-firewall.service \
          file://LICENSE \
          "

inherit systemd

RDEPENDS:${PN} = "nftables"

SYSTEMD_SERVICE:${PN} = "em-firewall.service"

do_install() {
    install -d ${D}${sysconfdir}/nftables.d/
    install -d ${D}${sbindir}
    install -d ${D}${systemd_unitdir}/system

    install -m 0644 ${WORKDIR}/00-em-firewall-init.conf ${D}${sysconfdir}/nftables.d/
    install -m 0755 ${WORKDIR}/em-firewall ${D}${sbindir}/em-firewall
    install -m 0644 ${WORKDIR}/em-firewall.service ${D}${systemd_unitdir}/system/
}
