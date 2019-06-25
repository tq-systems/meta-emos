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
	file://emcfg-generator \
	file://80-button-handler.rules \
"

SYSTEMD_SERVICE_${PN} = "em-app-flash-scan.timer"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${base_sbindir}
	install -m 0755 emcfg em-config-reset em-init em-update-password ${D}${base_sbindir}/

	install -d ${D}${sysconfdir}/tmpfiles.d
	install -m 0644 00-emos-log.conf ${D}${sysconfdir}/tmpfiles.d/

	install -d ${D}${sysconfdir}/sysctl.d
	install -m 0644 sysctl.conf ${D}${sysconfdir}/sysctl.d/80-emos.conf

	install -d ${D}${sysconfdir}/systemd/network
	ln -s /run/em/etc/systemd/timesyncd.conf ${D}${sysconfdir}/systemd/
	ln -s /run/em/etc/systemd/network/50-wired.network ${D}${sysconfdir}/systemd/network/

	install -d ${D}${systemd_unitdir}/system/sysinit.target.wants/
	install -m 0644 emcfg.service etc-shadow.mount em-app-flash-scan.timer ${D}${systemd_unitdir}/system/
	ln -s ../emcfg.service ../etc-shadow.mount ${D}${systemd_unitdir}/system/sysinit.target.wants/

	install -d ${D}${systemd_unitdir}/system-generators
	install -m 0755 emcfg-generator ${D}${systemd_unitdir}/system-generators/

	install -d ${D}${base_libdir}/udev/rules.d
	install -m 0644 ${WORKDIR}/80-button-handler.rules ${D}${base_libdir}/udev/rules.d/
}

RDEPENDS_${PN} += "jq u-boot-fslc-fw-utils"

FILES_${PN} += " \
	${sysconfdir}/tmpfiles.d/00-emos-log.conf \
	${sysconfdir}/systemd/network/50-wired.network \
	${systemd_unitdir}/system/em-update-password.service \
	${systemd_unitdir}/system/etc-shadow.mount \
	${systemd_unitdir}/system/emcfg.service \
	${systemd_unitdir}/system/sysinit.target.wants/etc-shadow.mount \
	${systemd_unitdir}/system/sysinit.target.wants/emcfg.service \
	${systemd_unitdir}/system-generators/emcfg-generator \
"

ALTERNATIVE_${PN} += "init"
ALTERNATIVE_TARGET[init] = "${base_sbindir}/em-init"
ALTERNATIVE_LINK_NAME[init] = "${base_sbindir}/init"
ALTERNATIVE_PRIORITY[init] ?= "900"
