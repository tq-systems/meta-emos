# Copyright (C) 2012-2017 O.S. Systems Software LTDA.
# Released under the MIT license (see COPYING.MIT for the terms)

inherit fsl-u-boot-localversion pythonnative

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=a2c678cfd4a4d97135585cad908541c6"

SRC_URI = " \
	git://github.com/tq-systems/u-boot-em.git;branch=${SRCBRANCH};protocol=https \
	file://fw_env.config \
"

SRCREV = "facc91b845b23949f6b54a54d0c376b994ade0f4"
SRCBRANCH = "EM3x0-v2017.03+fslc"

PV = "v2017.03+git${SRCPV}"

S = "${WORKDIR}/git"
