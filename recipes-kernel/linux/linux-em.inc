# Copyright (C) 2012, 2015 O.S. Systems Software LTDA.
# Released under the MIT license (see COPYING.MIT for the terms)

LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

inherit kernel kernel-yocto

SRCBRANCH ?= ""

SRC_URI = " \
	git://github.com/tq-systems/linux-em.git;branch=${SRCBRANCH};protocol=https \
	file://defconfig \
"

S = "${WORKDIR}/git"

DEPENDS += "lzop-native bc-native openssl-native"

# Set the PV to the correct kernel version to satisfy the kernel version sanity check
LINUX_VERSION ??= "invalid"
PV = "${LINUX_VERSION}+git${SRCPV}"

# This comment was copied from linux-imx recipes in meta-freescale:
# Tell to kernel class that we would like to use our defconfig to configure the kernel.
# Otherwise, the --allnoconfig would be used per default which leads to mis-configured
# kernel.
#
# This behavior happens when a defconfig is provided, the kernel-yocto configuration
# uses the filename as a trigger to use a 'allnoconfig' baseline before merging
# the defconfig into the build.
#
# If the defconfig file was created with make_savedefconfig, not all options are
# specified, and should be restored with their defaults, not set to 'n'.
# To properly expand a defconfig like this, we need to specify: KCONFIG_MODE="--alldefconfig"
# in the kernel recipe include.
KCONFIG_MODE="--alldefconfig"

