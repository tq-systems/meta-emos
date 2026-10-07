SUMMARY = "Clock change event monitor"
SECTION = "system/base"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
    file://configure.ac \
    file://Makefile.am \
    file://src/em-timeguard.c \
    file://src/em-timeguard.h \
"

S = "${WORKDIR}"
FILESEXTRAPATHS:prepend := "${THISDIR}/:"

# do_package hardlinks ${S}/src into the debug sources; unpacking over them in place makes pseudo abort.
do_unpack[cleandirs] += "${S}/src"

DEPENDS = "systemd"

inherit autotools pkgconfig
