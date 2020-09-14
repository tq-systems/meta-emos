do_install_append_emos() {
	# Set watchdog timeout
	sed -i -e 's/.*RuntimeWatchdogSec.*/RuntimeWatchdogSec=29/' ${D}${sysconfdir}/systemd/system.conf

	# Set configurations in journald.conf
	sed -i \
		-e 's/.*RateLimitIntervalSec.*/RateLimitIntervalSec=5m/' \
		-e 's/.*RateLimitBurst.*/RateLimitBurst=100/' \
		-e 's/.*SystemMaxUse.*/SystemMaxUse=80M/' \
		-e 's/.*SystemKeepFree.*/SystemKeepFree=16M/' \
		-e 's/.*MaxLevelStore.*/MaxLevelStore=notice/' \
		${D}${sysconfdir}/systemd/journald.conf

	# We manage timesyncd enable status in emcfg
	rm ${D}${sysconfdir}/systemd/timesyncd.conf
	sed -i -e '/^enable systemd-timesyncd\.service$/d' ${D}${systemd_unitdir}/system-preset/90-systemd.preset

	# Avoid syslog warnings about non-existing "render" group
	sed -i -e '/\<GROUP="render"/d' ${D}${rootlibexecdir}/udev/rules.d/50-udev-default.rules
}
