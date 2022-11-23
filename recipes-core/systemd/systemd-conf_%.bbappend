FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "\
    file://10-journald-emos.conf \
    file://10-logind-emos.conf \
    file://10-system-emos.conf \
"

do_install:append:emos() {
	rm -r ${D}${systemd_unitdir}/network/*.network

	install -D -m0644 ${WORKDIR}/10-journald-emos.conf ${D}${systemd_unitdir}/journald.conf.d/
	install -D -m0644 ${WORKDIR}/10-logind-emos.conf ${D}${systemd_unitdir}/logind.conf.d/
	install -D -m0644 ${WORKDIR}/10-system-emos.conf ${D}${systemd_unitdir}/system.conf.d/

}
