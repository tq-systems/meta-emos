SUMARRY=" Intelligently block brute-force attacks by aggregating system logs "
HOMEPAGE = "https://www.sshguard.net/"
LIC_FILES_CHKSUM = "file://COPYING;md5=47a33fc98cd20713882c4d822a57bf4d"
LICENSE = "BSD-1-Clause"

SRC_URI="git://bitbucket.org/sshguard/sshguard.git;protocol=https;branch=master"
SRCREV = "613264cd33b5cbe91338532353dfedd0722ada8f"

DEPENDS = " flex-native bison-native python3-docutils-native"

inherit autotools-brokensep

inherit systemd

SRC_URI += " \
	file://sshguard.service \
	file://sshguard.conf \
"

S = "${WORKDIR}/git"

SYSTEMD_SERVICE:${PN} = "sshguard.service"

do_install:append() {
    install -d ${D}${sysconfdir}
    install -d ${D}${systemd_unitdir}/system

    install -m 0755 ${WORKDIR}/sshguard.conf ${D}/${sysconfdir}
    install -m 0644 ${WORKDIR}/sshguard.service ${D}${systemd_unitdir}/system/
}
