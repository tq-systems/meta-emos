DESCRIPTION="Energy Manager network configuration"

LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

SRC_URI = " \
	file://LICENSE \
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
