FILESEXTRAPATHS:prepend:emos := "${THISDIR}/files:"
SYSTEMD_AUTO_ENABLE:${PN} = "disable"

SRC_URI:append:emos = " \
	file://dropbearkey-emos.conf \
"

do_install:append:emos() {
	sed -r -i \
		-e '/^ConditionPathExists=!\/etc/d' \
		-e 's@^(ConditionPathExists=!)(/var/lib/dropbear/dropbear_rsa_host_key)$@\1/cfglog\2@' \
		${D}${systemd_unitdir}/system/dropbearkey.service

	install -d ${D}${systemd_unitdir}/system/dropbearkey.service.d/
	install -m 0644 ${WORKDIR}/dropbearkey-emos.conf ${D}${systemd_unitdir}/system/dropbearkey.service.d/

}

FILES:${PN}:append:emos = "\
	${systemd_unitdir}/system/dropbearkey.service.d/ \
"
