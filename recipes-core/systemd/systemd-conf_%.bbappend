do_install_append_emos() {
	rm -r ${D}${systemd_unitdir}/network/*.network
}
