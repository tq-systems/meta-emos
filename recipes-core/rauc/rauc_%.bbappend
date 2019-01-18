FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

do_install_append() {
	sed -i "s^@MACHINE@^${MACHINE}^g" ${D}${sysconfdir}/rauc/system.conf
}
