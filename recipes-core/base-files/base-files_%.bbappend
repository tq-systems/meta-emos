FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

do_install_append () {
	# Install mountpoints
	install -d ${D}/apps
	install -d ${D}/auth
	install -d ${D}/cfglog
	install -d ${D}/data
	install -d ${D}/update

	install -d ${D}${localstatedir}/lib/private

	rm -rf ${D}${localstatedir}/log
	ln -s /cfglog/var/log ${D}${localstatedir}/log

}

FILES_${PN} += "/apps /auth /cfglog /data /update"
