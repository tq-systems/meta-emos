LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${S}/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

inherit cmake

DEPENDS = "jansson"

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/tq-systems/libdeviceinfo-em.git;protocol=https;branch=master"
SRCREV = "90fb2a50553e64957a34613dc0a3e3e18f6d1ef1"
