FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append:emos = " \
	file://mke2fs.conf \
"

do_install:append () {
	install	-m 0644 ${WORKDIR}/mke2fs.conf ${D}/${sysconfdir}/
}
