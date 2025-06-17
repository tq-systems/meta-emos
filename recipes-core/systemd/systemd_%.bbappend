include systemd.inc

FILESEXTRAPATHS:prepend:emos := "${THISDIR}/files:"
SRC_URI:append:emos = "\
	file://0001-shared-dropin-disable-unit-name-prefix-dropins.patch \
	file://0002-shared-dropin-disable-support-for-toplevel-unit-type.patch \
	file://0003-basic-linux-update-kernel-headers-from-v6.8-rc5.patch \
	file://0004-network-bridge-add-support-for-IFLA_BR_FDB_MAX_LEARN.patch \
	file://0005-networkctl-add-support-to-display-learned-fdb-entrie.patch \
	file://0006-test-systemd-networkd-tests-add-fdb-learned-tests.patch \
	file://0007-udev-builtin-net_id-add-NAMING_DEVICETREE_PORT_ALIAS.patch \
	\
	file://read-only-rootfs.conf \
	file://set-timeout-stop.conf \
"

pkg_postinst:${PN}:libc-glibc:append:emos () {
	# Add systemd to nsswitch.conf
	sed -e '/passwd:/ s/$/ systemd/' \
		-e '/group:/ s/$/ systemd/' \
		-i $D${sysconfdir}/nsswitch.conf
}

PACKAGECONFIG:append:emos = " cgroupv2 sysusers xz"
EXTRA_OEMESON:append:emos = " -Dpstore=false"

do_install:append:emos() {
	# We manage timesyncd enable status in emcfg
	rm ${D}${sysconfdir}/systemd/timesyncd.conf
	sed -i -e '/^enable systemd-timesyncd\.service$/d' ${D}${systemd_unitdir}/system-preset/90-systemd.preset

	# Add configurations for read-only root filesystem
	install -m755 -d \
		${D}${systemd_system_unitdir}/systemd-logind.service.d \
		${D}${systemd_system_unitdir}/systemd-timesyncd.service.d

	install -m644 ${WORKDIR}/read-only-rootfs.conf \
		${D}${systemd_system_unitdir}/systemd-logind.service.d

	install -m644 ${WORKDIR}/read-only-rootfs.conf \
		${D}${systemd_system_unitdir}/systemd-timesyncd.service.d
	install -m644 ${WORKDIR}/set-timeout-stop.conf \
		${D}${systemd_system_unitdir}/systemd-timesyncd.service.d

	sed -i 's|^d /var/log .*|L /var/log - - - - /cfglog/var/log|' \
	    ${D}${nonarch_libdir}/tmpfiles.d/var.conf
}
