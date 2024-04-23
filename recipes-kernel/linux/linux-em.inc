# Copyright (C) 2012, 2015 O.S. Systems Software LTDA.
# Released under the MIT license (see COPYING.MIT for the terms)

LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

inherit kernel

SRCBRANCH ?= ""

SRC_URI = " \
	git://github.com/tq-systems/linux-em.git;branch=${SRCBRANCH};protocol=https \
	file://defconfig \
"

S = "${WORKDIR}/git"

DEPENDS += "lzop-native bc-native"
