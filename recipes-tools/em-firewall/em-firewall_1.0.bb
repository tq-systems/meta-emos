DESCRIPTION = "Energy Manager Firewall"

LICENSE = "TQSPSLA-1.0.3"
LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSPSLA-1.0.3')};md5=675d9988badfa6f03ad1d2678a0d50b3"

SRC_URI = " \
          file://00-em-firewall-init.conf \
          file://em-firewall \
          file://em-firewall.service \
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
