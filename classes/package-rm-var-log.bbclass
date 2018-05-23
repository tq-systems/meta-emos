package_rm_var_log() {
	# Remove /var/log, as it will clash with the symlink to /data/var/log
	# created by base-files
	if [ ! -L ${PKGD}${localstatedir}/log ]; then
		rm -rf ${PKGD}${localstatedir}/log
	fi
}

PACKAGE_PREPROCESS_FUNCS += "package_rm_var_log"
