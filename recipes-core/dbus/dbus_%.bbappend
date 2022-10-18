EXTRA_OECONF:append = " --runstatedir=/run"

do_install:append() {
	if ${@bb.utils.contains('DISTRO_FEATURES', 'sysvinit', 'true', 'false', d)}; then
		sed -i 's/\${localstatedir}\/run\/dbus/\/run\/dbus/' ${D}${sysconfdir}/default/volatiles/99_dbus
	fi
}
