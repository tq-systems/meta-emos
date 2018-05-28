# Copyright (C) 2012-2017 O.S. Systems Software LTDA.
# Released under the MIT license (see COPYING.MIT for the terms)

inherit fsl-u-boot-localversion

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=a2c678cfd4a4d97135585cad908541c6"

SRC_URI = "${GIT_SERVER}/base/SW.u-boot.git;branch=${SRCBRANCH};protocol=ssh"

SRCBRANCH_em300 = 'EM300-v2017.03+fslc'
SRCREV_em300 = 'dfbfdd4bc05608fae57dd993b2101fd382672430'

SRCBRANCH_em310 = 'EM310-v2017.03+fslc'
SRCREV_em310 = '84ca5c591f8667a0eaceef731b90bdf78155babd'

PV = "v2017.03+git${SRCPV}"

S = "${WORKDIR}/git"
