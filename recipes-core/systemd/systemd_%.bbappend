FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI += "\
	file://read-only-rootfs.conf \
"

PACKAGECONFIG:append:pn-systemd = " cgroupv2 sysusers"
EXTRA_OEMESON += "-Dpstore=false"

do_install:append:emos() {
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

	# Do not restart when Reset-Button is pressed
	sed -i -e 's/.*HandleRebootKey.*/HandleRebootKey=ignore/' ${D}${sysconfdir}/systemd/logind.conf

	# We manage timesyncd enable status in emcfg
	rm ${D}${sysconfdir}/systemd/timesyncd.conf
	sed -i -e '/^enable systemd-timesyncd\.service$/d' ${D}${systemd_unitdir}/system-preset/90-systemd.preset

	# Avoid syslog warnings about non-existing "render" group
	sed -i -e '/\<GROUP="render"/d' ${D}${rootlibexecdir}/udev/rules.d/50-udev-default.rules

	# Add configurations for read-only root filesystem
	install -m755 -d \
		${D}${systemd_system_unitdir}/systemd-logind.service.d \
		${D}${systemd_system_unitdir}/systemd-timesyncd.service.d

	install -m644 ${WORKDIR}/read-only-rootfs.conf \
		${D}${systemd_system_unitdir}/systemd-logind.service.d

	install -m644 ${WORKDIR}/read-only-rootfs.conf \
		${D}${systemd_system_unitdir}/systemd-timesyncd.service.d
}
