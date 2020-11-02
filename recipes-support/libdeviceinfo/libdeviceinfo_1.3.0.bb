LICENSE = "TQSSLA_V1.0.2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/git/LICENSE;md5=5a77156d011829e57ffe26e62f07ff2d"
SRC_DISTRIBUTE_LICENSES += "TQSSLA_V1.0.2"

inherit cmake

DEPENDS = "jansson"

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/tq-systems/libdeviceinfo-em.git;protocol=https"
SRCREV = "7edfd959e88cac67184056d67ae6f38282d462df"
