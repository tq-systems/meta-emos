SYSTEMD_AUTO_ENABLE_${PN} = "disable"

do_install_append() {
	sed -r -i \
		-e '/^ConditionPathExists=!\/etc/d' \
		-e 's@^(ConditionPathExists=!)(/var/lib/dropbear/dropbear_rsa_host_key)$@\1/cfglog\2@' \
		${D}${systemd_unitdir}/system/dropbearkey.service
}
