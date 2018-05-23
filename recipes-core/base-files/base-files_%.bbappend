FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

inherit update-alternatives

SRC_URI += " \
            file://tmpfiles.conf \
            file://emos-init \
           "

do_install_append () {
	# Install mountpoints
	install -d ${D}/auth
	install -d ${D}/config
	install -d ${D}/data

	rm -rf ${D}${localstatedir}/log
	ln -s /data/var/log ${D}${localstatedir}/log

	install -d ${D}${base_sbindir}
	install -m 0755 ${WORKDIR}/emos-init ${D}${base_sbindir}/emos-init

	install -d ${D}${sysconfdir}/tmpfiles.d
	install -m 0644 ${WORKDIR}/tmpfiles.conf ${D}${sysconfdir}/tmpfiles.d/00-emos-log.conf
}

FILES_${PN} += "/auth /config /data ${base_sbindir}/emos-init ${sysconfdir}/tmpfiles.d/00-emos-log.conf"

ALTERNATIVE_${PN} += "init"
ALTERNATIVE_TARGET[init] = "${base_sbindir}/emos-init"
ALTERNATIVE_LINK_NAME[init] = "${base_sbindir}/init"
ALTERNATIVE_PRIORITY[init] ?= "900"
