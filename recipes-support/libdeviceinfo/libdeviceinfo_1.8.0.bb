LICENSE = "TQSPSLA-1.0.3"
LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSPSLA-1.0.3')};md5=675d9988badfa6f03ad1d2678a0d50b3"
SRC_DISTRIBUTE_LICENSES += "TQSPSLA-1.0.3"

inherit cmake

DEPENDS = "jansson"

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/tq-systems/libdeviceinfo-em.git;protocol=https;branch=master"
SRCREV = "31d37feab8d632c519ac9a1159593dcd28aade37"

BBCLASSEXTEND = "nativesdk"
