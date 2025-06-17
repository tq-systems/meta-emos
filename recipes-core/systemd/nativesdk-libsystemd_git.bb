SUMMARY = "libsystemd shared library for nativesdk"
HOMEPAGE = "http://www.freedesktop.org/wiki/Software/systemd"

include systemd.inc

SECTION = "libs"

DESCRIPTION = "libsystemd shared library built specifically for nativesdk to use for sdbus-c++"

LICENSE = "LGPL-2.1-or-later"
LIC_FILES_CHKSUM = "file://LICENSE.LGPL2.1;md5=4fbd65380cdd255951079008b364516c"

S = "${WORKDIR}/git"

DEPENDS = "gperf-native libcap util-linux python3-jinja2-native ninja"

inherit meson pkgconfig nativesdk

do_compile() {
    ninja -v ${PARALLEL_MAKE} libsystemd.so.0.38.0
}

do_install () {
    install -d ${D}${libdir}
    install ${B}/libsystemd.so.0.38.0 ${D}${libdir}
    ln -sf libsystemd.so.0.38.0 ${D}${libdir}/libsystemd.so.0
    ln -sf libsystemd.so.0.38.0 ${D}${libdir}/libsystemd.so

    install -d ${D}${includedir}/systemd
    install ${S}/src/systemd/*.h ${D}${includedir}/systemd
}
