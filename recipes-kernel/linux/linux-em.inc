# Copyright (C) 2012, 2015 O.S. Systems Software LTDA.
# Released under the MIT license (see COPYING.MIT for the terms)

LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

inherit kernel fsl-kernel-localversion

# Put a local version until we have a true SRCREV to point to
LOCALVERSION = "-em"
SCMVERSION ?= "y"
SRCBRANCH ?= ""

SRC_URI = " \
	git://github.com/tq-systems/linux-em.git;branch=${SRCBRANCH};protocol=https \
	file://defconfig \
"

S = "${WORKDIR}/git"

# We need to pass it as param since kernel might support more then one
# machine, with different entry points
KERNEL_EXTRA_ARGS += "LOADADDR=${UBOOT_ENTRYPOINT}"

DEPENDS += "lzop-native bc-native"
