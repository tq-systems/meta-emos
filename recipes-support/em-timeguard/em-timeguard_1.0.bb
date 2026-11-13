SUMMARY = "Clock change event monitor"
SECTION = "system/base"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
    file://configure.ac \
    file://Makefile.am \
    file://src/em-timeguard.c \
    file://src/em-timeguard.h \
    file://com.tq_group.tq_em.timeguard1.xml \
    file://com.tq_group.tq_em.timeguard1.conf \
    file://em-timeguard.service \
"

S = "${WORKDIR}"
FILESEXTRAPATHS:prepend := "${THISDIR}/:"

# do_package hardlinks ${S}/src into the debug sources; unpacking over them in place makes pseudo abort.
do_unpack[cleandirs] += "${S}/src"

DEPENDS = "systemd"

inherit autotools pkgconfig systemd useradd

SYSTEMD_SERVICE:${PN} = "em-timeguard.service"
USERADD_PACKAGES = "${PN}"
GROUPADD_PARAM:${PN} = "--system em-timeguard"
USERADD_PARAM:${PN} = "--system -M -s /bin/nologin -g em-timeguard em-timeguard"

do_install:append() {
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/em-timeguard.service \
        ${D}${systemd_system_unitdir}/
    install -d ${D}${sysconfdir}/dbus-1/system.d
    install -m 0644 ${WORKDIR}/com.tq_group.tq_em.timeguard1.conf \
        ${D}${sysconfdir}/dbus-1/system.d/
    install -d ${D}${datadir}/dbus-1/interfaces
    install -m 0644 ${WORKDIR}/com.tq_group.tq_em.timeguard1.xml \
        ${D}${datadir}/dbus-1/interfaces/
}

FILES:${PN} += " \
     ${sysconfdir}/dbus-1/system.d/com.tq_group.tq_em.timeguard1.conf \
     ${datadir}/dbus-1/interfaces/com.tq_group.tq_em.timeguard1.xml \
"
