# Copyright (C) 2012-2017 O.S. Systems Software LTDA.
# Released under the MIT license (see COPYING.MIT for the terms)

inherit fsl-u-boot-localversion

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=a2c678cfd4a4d97135585cad908541c6"

SRC_URI = "${GIT_SERVER}/base/SW.u-boot.git;branch=${SRCBRANCH};protocol=ssh"

SRCBRANCH = 'EM3x0-v2017.03+fslc'
SRCREV = '1a0a82bb54164c94b4537017e325f47077f0b031'

PV = "v2017.03+git${SRCPV}"

S = "${WORKDIR}/git"
