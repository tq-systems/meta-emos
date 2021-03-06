DESCRIPTION="Energy Manager configuration handler"

LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

inherit update-alternatives systemd

SRC_URI = " \
	file://LICENSE \
	file://emcfg \
	file://em-config-reset \
	file://em-init \
	file://em-update-password \
	file://00-emos-log.conf \
	file://sysctl.conf \
	file://emcfg.service \
	file://etc-shadow.mount \
	file://em-app-flash-scan.timer \
	file://em-log-fsck-errors.service \
	file://em-timesync \
	file://em-timesync-timer \
	file://em-timesync-timer.service \
	file://emos.target \
	file://emcfg-generator \
	file://80-button-handler.rules \
	file://em-keygen \
	file://openssl-em.cnf \
	file://journald-debug.conf \
"

SYSTEMD_SERVICE_${PN} = " \
	em-app-flash-scan.timer \
	em-log-fsck-errors.service \
	em-timesync-timer.service \
"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${base_sbindir}
	install -m 0755 \
		emcfg \
		em-config-reset \
		em-init \
		em-update-password \
		em-timesync \
		em-timesync-timer \
		${D}${base_sbindir}/

	install -d ${D}${sysconfdir}/tmpfiles.d
	install -m 0644 00-emos-log.conf ${D}${sysconfdir}/tmpfiles.d/

	install -d ${D}${sysconfdir}/sysctl.d
	install -m 0644 sysctl.conf ${D}${sysconfdir}/sysctl.d/80-emos.conf

	install -d ${D}${sysconfdir}/systemd
	ln -s /run/em/etc/systemd/timesyncd.conf ${D}${sysconfdir}/systemd/

	install -d ${D}${systemd_unitdir}/system/sysinit.target.wants/
	install -m 0644 \
		emcfg.service \
		etc-shadow.mount \
		em-app-flash-scan.timer \
		em-log-fsck-errors.service \
		em-timesync-timer.service \
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

	install -d ${D}${sysconfdir}/ssl
	install -m 0644 ${WORKDIR}/openssl-em.cnf ${D}${sysconfdir}/ssl/

	install -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/em-keygen ${D}${bindir}/

	install -d ${D}${sysconfdir}/systemd/journald.conf.d
	install -m 0644 ${WORKDIR}/journald-debug.conf ${D}${sysconfdir}/systemd/journald.conf.d/
}

RDEPENDS_${PN} += "jq libubootenv-bin openssl-bin faketime em-network-config"

FILES_${PN} += " \
	${sysconfdir}/tmpfiles.d/00-emos-log.conf \
	${systemd_unitdir}/system/ \
	${systemd_unitdir}/system-generators/emcfg-generator \
"

ALTERNATIVE_${PN} += "init"
ALTERNATIVE_TARGET[init] = "${base_sbindir}/em-init"
ALTERNATIVE_LINK_NAME[init] = "${base_sbindir}/init"
ALTERNATIVE_PRIORITY[init] ?= "900"
