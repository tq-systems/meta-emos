FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI += " file://50-wired.network"

do_install_append() {
	install -d ${D}${sysconfdir}/systemd/network
	install -m 0644 ${WORKDIR}/50-wired.network ${D}${sysconfdir}/systemd/network/50-wired.network
}

FILES_${PN} += "${sysconfdir}/systemd/network ${sysconfdir}/systemd/network/50-wired.network"
