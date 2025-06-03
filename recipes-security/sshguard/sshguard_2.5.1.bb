SUMARRY=" Intelligently block brute-force attacks by aggregating system logs "
HOMEPAGE = "https://www.sshguard.net/"
LIC_FILES_CHKSUM = "file://COPYING;md5=47a33fc98cd20713882c4d822a57bf4d"
LICENSE = "BSD-1-Clause"

SRC_URI="git://bitbucket.org/sshguard/sshguard.git;protocol=https;branch=master"
SRCREV = "efc45b818f8f7e0b61978948842f1b2fd9ae2c1e"

DEPENDS = " flex-native bison-native python3-docutils-native"

inherit autotools-brokensep

inherit systemd

SRC_URI += " \
	file://0001-Add-signature-for-dropbear-ssh-logs.patch \
	file://0002-sshguard.in-Fix-race-condition-in-exit-trap.patch \
	\
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
