DESCRIPTION="Energy Manager network configuration"

LICENSE = "TQSPSLA-1.0.3"
LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSPSLA-1.0.3')};md5=675d9988badfa6f03ad1d2678a0d50b3"

SRC_URI = " \
	file://br0.netdev \
	file://switch-br0-ports.network \
	file://switch-end0.network \
	file://single-br0-ports.network \
	file://double-br0-ports.network \
"

do_install() {
	install -d ${D}${sysconfdir}/systemd/network
	install -m644 ${WORKDIR}/br0.netdev ${D}${sysconfdir}/systemd/network/

	install -d ${D}${libdir}/emos/network-config/switch
	install -m644 ${WORKDIR}/switch-br0-ports.network ${D}${libdir}/emos/network-config/switch/br0-ports.network
	install -m644 ${WORKDIR}/switch-end0.network ${D}${libdir}/emos/network-config/switch/end0.network

	install -d ${D}${libdir}/emos/network-config/single
	install -m644 ${WORKDIR}/single-br0-ports.network ${D}${libdir}/emos/network-config/single/br0-ports.network

	install -d ${D}${libdir}/emos/network-config/double
	install -m644 ${WORKDIR}/double-br0-ports.network ${D}${libdir}/emos/network-config/double/br0-ports.network
}

FILES:${PN} += " \
	${sysconfdir}/systemd/network/ \
	${libdir}/emos/network-config/ \
"

COMPATIBLE_MACHINE = "em"
