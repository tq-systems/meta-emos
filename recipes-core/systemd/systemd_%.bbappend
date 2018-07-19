FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI += " file://50-wired.network"

do_install_append() {
	# Get rid of /etc/udev/hwdb.bin in overlay
	rm ${D}${systemd_unitdir}/system/sysinit.target.wants/systemd-hwdb-update.service

	install -d ${D}${sysconfdir}/systemd/network
	install -m 0644 ${WORKDIR}/50-wired.network ${D}${sysconfdir}/systemd/network/50-wired.network
}

FILES_${PN} += "${sysconfdir}/systemd/network ${sysconfdir}/systemd/network/50-wired.network"

# Ensure these files end up in systemd-container rather than systemd - otherwise
# systemd will have an unwanted dependency on systemd-container!
FILES_${PN}-container += "\
	${sysconfdir}/systemd/system/multi-user.target.wants/machines.target \
	${systemd_system_unitdir}/machines.target.wants/var-lib-machines.mount \
	${systemd_system_unitdir}/remote-fs.target.wants/var-lib-machines.mount \
"
