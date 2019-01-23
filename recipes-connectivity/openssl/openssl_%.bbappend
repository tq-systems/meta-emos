FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append_emos = " \
	file://openssl-em.cnf \
"

do_install_append_emos() {
	install -d ${D}${sysconfdir}/ssl/

	install -m 0644 ${WORKDIR}/openssl-em.cnf		${D}${sysconfdir}/ssl/
}
