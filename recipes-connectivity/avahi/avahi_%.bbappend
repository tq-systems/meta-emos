do_install_append_emos() {
	rm ${D}${sysconfdir}/avahi/services/ssh.service
	rm ${D}${sysconfdir}/avahi/services/sftp-ssh.service
}
