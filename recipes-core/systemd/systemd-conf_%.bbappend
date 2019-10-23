do_install_append_emos() {
	# Set watchdog timeout
	sed -i -e 's/.*RuntimeWatchdogSec.*/RuntimeWatchdogSec=29/' ${D}${sysconfdir}/systemd/system.conf
}
