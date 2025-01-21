DESCRIPTION="Energy Manager configuration handler"

LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

inherit update-alternatives systemd useradd

# These variables have to be set for class "useradd"
USERADD_PACKAGES = "${PN}"
GROUPADD_PARAM:${PN} += "--system em-group-data;"
GROUPADD_PARAM:${PN} += "--system em-group-reboot;"
GROUPADD_PARAM:${PN} += "--system em-group-update;"
GROUPADD_PARAM:${PN} += "--system em-group-sudo-fw_printenv;"
GROUPADD_PARAM:${PN} += "--system em-group-sudo-systemctl_restart;"
GROUPADD_PARAM:${PN} += "--system em-group-sudo-systemctl_stop;"
GROUPADD_PARAM:${PN} += "--system em-group-sudo-cat_device_key;"
GROUPADD_PARAM:${PN} += "--system em-group-cfglog;"
GROUPADD_PARAM:${PN} += "--system em-group-gpio;"
GROUPADD_PARAM:${PN} += "--system em-group-spi;"


SRC_URI = " \
	file://LICENSE \
	file://emcfg \
	file://em-appctl \
	file://em-config-reset \
	file://em-init \
	file://em-update-password \
	file://00-emos-log.conf \
	file://sysctl.conf \
	file://emcfg.service \
	file://etc-shadow.mount \
	file://em-app-flash-scan.timer \
	file://em-log-fsck-errors.service \
	file://emos.target \
	file://emcfg-generator \
	file://80-button-handler.rules \
	file://em-keygen \
	file://openssl-em.cnf \
	file://journald-debug.conf \
	file://80-ttyAPP.rules \
	file://90-gpio-teridian.rules \
"

SYSTEMD_SERVICE:${PN} = " \
	em-app-flash-scan.timer \
	em-log-fsck-errors.service \
"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${base_sbindir}
	install -m 0755 \
		emcfg \
		em-appctl \
		em-config-reset \
		em-init \
		em-update-password \
		${D}${base_sbindir}/

	install -d ${D}${sysconfdir}/tmpfiles.d
	install -m 0644 00-emos-log.conf ${D}${sysconfdir}/tmpfiles.d/

	install -d ${D}${sysconfdir}/sysctl.d
	install -m 0644 sysctl.conf ${D}${sysconfdir}/sysctl.d/80-emos.conf

	install -d ${D}${systemd_unitdir}/system/sysinit.target.wants/
	install -m 0644 \
		emcfg.service \
		etc-shadow.mount \
		em-app-flash-scan.timer \
		em-log-fsck-errors.service \
		emos.target \
		${D}${systemd_unitdir}/system/
	ln -s \
		../emcfg.service \
		../etc-shadow.mount \
		../em-log-fsck-errors.service \
		${D}${systemd_unitdir}/system/sysinit.target.wants/

	install -d ${D}${systemd_unitdir}/system-generators
	install -m 0755 emcfg-generator ${D}${systemd_unitdir}/system-generators/

	install -d ${D}${base_libdir}/udev/rules.d
	install -m 0644 ${WORKDIR}/80-button-handler.rules ${D}${base_libdir}/udev/rules.d/
	install -m 0644 ${WORKDIR}/80-ttyAPP.rules ${D}${base_libdir}/udev/rules.d/
	install -m 0644 ${WORKDIR}/90-gpio-teridian.rules ${D}${base_libdir}/udev/rules.d/

	install -d ${D}${sysconfdir}/ssl
	install -m 0644 ${WORKDIR}/openssl-em.cnf ${D}${sysconfdir}/ssl/

	install -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/em-keygen ${D}${bindir}/

	install -d ${D}${sysconfdir}/systemd/journald.conf.d
	install -m 0644 ${WORKDIR}/journald-debug.conf ${D}${sysconfdir}/systemd/journald.conf.d/

	# Add sudo accesses for user.
	install -d -m 0755 "${D}/etc/sudoers.d"
	echo "%em-group-reboot ALL=(ALL) NOPASSWD: /sbin/reboot" > "${D}/etc/sudoers.d/reboot"
	echo "%em-group-sudo-fw_printenv ALL=(ALL) NOPASSWD: /usr/bin/fw_printenv" > "${D}/etc/sudoers.d/fw_printenv"
	echo "%em-group-sudo-systemctl_restart ALL=(ALL) NOPASSWD: /bin/systemctl restart *" > "${D}/etc/sudoers.d/systemctl_restart"
	echo "%em-group-sudo-systemctl_stop ALL=(ALL) NOPASSWD: /bin/systemctl stop *" > "${D}/etc/sudoers.d/systemctl_stop"
	echo "%em-group-sudo-cat_device_key ALL=(ALL) NOPASSWD: /bin/cat /auth/etc/default/auth/device.key" > "${D}/etc/sudoers.d/cat_device_key"
	echo "%em-group-sudo-cat_device_key ALL=(ALL) NOPASSWD: /bin/cat /auth/etc/default/auth/device.crt" >> "${D}/etc/sudoers.d/cat_device_key"
	chmod 0440 "${D}/etc/sudoers.d/"*

	# Install mountpoints
	install -d ${D}/apps
	install -d ${D}/auth
	install -d ${D}/cfglog
	install -d ${D}/data
	install -d ${D}/update

	install -d ${D}${localstatedir}/lib/private

	ln -s /cfglog/var/log ${D}${localstatedir}/log
}

RDEPENDS:${PN} += "jq libubootenv-bin openssl-bin libfaketime em-network-config udev"

FILES:${PN} += " \
	${sysconfdir}/tmpfiles.d/00-emos-log.conf \
	${systemd_unitdir}/system/ \
	${systemd_unitdir}/system-generators/emcfg-generator \
	${base_libdir}/udev/rules.d/ \
	/etc/sudoers.d \
	/apps \
	/auth \
	/cfglog \
	/data \
	/update \
"

ALTERNATIVE:${PN} += "init"
ALTERNATIVE_TARGET[init] = "${base_sbindir}/em-init"
ALTERNATIVE_LINK_NAME[init] = "${base_sbindir}/init"
ALTERNATIVE_PRIORITY[init] ?= "900"
