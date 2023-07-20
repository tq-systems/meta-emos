do_install:append () {
	# the log-directory is created in the emcfg package
	rm -rf ${D}${localstatedir}/log
}
