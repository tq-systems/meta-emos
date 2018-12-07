DESCRIPTION="Energy Manager configuration handler"

LICENSE = "TQSSLA_V1.0.1"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=52c97447b5d8ae219abdddeb738b3140"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.1"

inherit update-alternatives

SRC_URI = " \
	file://LICENSE \
	file://emcfg \
	file://em-init \
	file://em-update-password \
	file://00-emos-log.conf \
	file://sysctl.conf \
	file://emcfg.service \
	file://etc-shadow.mount \
	file://emcfg-generator \
"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${base_sbindir}
	install -m 0755 emcfg em-init em-update-password ${D}${base_sbindir}/

	install -d ${D}${sysconfdir}/tmpfiles.d
	install -m 0644 00-emos-log.conf ${D}${sysconfdir}/tmpfiles.d/

	install -d ${D}${sysconfdir}/sysctl.d
	install -m 0644 sysctl.conf ${D}${sysconfdir}/sysctl.d/80-emos.conf

	install -d ${D}${sysconfdir}/systemd/network
	ln -s /run/em/etc/systemd/timesyncd.conf ${D}${sysconfdir}/systemd/
	ln -s /run/em/etc/systemd/network/50-wired.network ${D}${sysconfdir}/systemd/network/

	install -d ${D}${systemd_unitdir}/system/sysinit.target.wants/
	install -m 0644 emcfg.service etc-shadow.mount ${D}${systemd_unitdir}/system/
	ln -s ../emcfg.service ../etc-shadow.mount ${D}${systemd_unitdir}/system/sysinit.target.wants/

	install -d ${D}${systemd_unitdir}/system-generators
	install -m 0755 emcfg-generator ${D}${systemd_unitdir}/system-generators/
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
