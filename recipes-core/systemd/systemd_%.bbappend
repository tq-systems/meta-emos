do_install_append() {
	# We manage timesyncd enable status in emcfg
	rm ${D}${sysconfdir}/systemd/timesyncd.conf
	rm ${D}${sysconfdir}/systemd/system/sysinit.target.wants/systemd-timesyncd.service
}

# Ensure these files end up in systemd-container rather than systemd - otherwise
# systemd will have an unwanted dependency on systemd-container!
FILES_${PN}-container += "\
	${sysconfdir}/systemd/system/multi-user.target.wants/machines.target \
	${systemd_system_unitdir}/machines.target.wants/var-lib-machines.mount \
	${systemd_system_unitdir}/remote-fs.target.wants/var-lib-machines.mount \
"
