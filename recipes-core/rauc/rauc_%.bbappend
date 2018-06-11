FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

RAUC_KEYRING_FILE ?= "${@bb.utils.which(d.getVar('BBPATH'), 'files/emos/rauc/ca.cert.pem')}"

do_install_append() {
	sed -i "s^@MACHINE@^${MACHINE}^g" ${D}${sysconfdir}/rauc/system.conf
}
