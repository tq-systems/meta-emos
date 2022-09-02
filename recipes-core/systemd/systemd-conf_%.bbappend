do_install:append:emos() {
	rm -r ${D}${systemd_unitdir}/network/*.network
}
