do_install_append() {
	# Set watchdog timeout
	sed -i -e 's/.*RuntimeWatchdogSec.*/RuntimeWatchdogSec=20/' ${D}${sysconfdir}/systemd/system.conf
}
