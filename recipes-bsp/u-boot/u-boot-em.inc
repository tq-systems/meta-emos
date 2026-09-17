# SPDX-License-Identifier: MIT
#
# Copyright (C) 2024 TQ-Systems GmbH <oss@ew.tq-group.com>, D-82229 Seefeld, Germany.
# Author: Matthias Schiffer
#
# Imported from meta-ti-bsp and stripped down, which is also MIT-licensed, without
# mention of a specific copyright holder.

# The original file can't be used, as it doesn't deploy u-boot-initial-env
# to DEPLOY_DIR_IMAGE.

do_compile:prepend () {
    scm_version=$(printf '%s%s' -em-g $(git -C ${S} rev-parse --verify HEAD 2>/dev/null | cut -c1-12))

    echo ${scm_version} > ${B}/.scmversion
    echo ${scm_version} > ${S}/.scmversion
}

require ${COREBASE}/meta/recipes-bsp/u-boot/u-boot-common.inc
require ${COREBASE}/meta/recipes-bsp/u-boot/u-boot.inc

PV:append = "+git${SRCPV}"

CVE_PRODUCT = "denx:u-boot"

# u-boot needs devtree compiler to parse dts files
DEPENDS += "dtc-native bc-native gnutls-native lzop-native flex-native bison-native python3-setuptools-native"

# u-boot needs these for binman
DEPENDS += "python3-pyelftools-native python3-pyyaml-native python3-jsonschema-native python3-yamllint-native"

DM_FIRMWARE = "ipc_echo_testb_mcu1_0_release_strip.xer5f"

PLAT_SFX = ""

PACKAGECONFIG[atf] = "BL31=${STAGING_DIR_HOST}/firmware/bl31.bin,,trusted-firmware-a"
PACKAGECONFIG:append:aarch64 = " atf"

BINMAN_INDIRS="${STAGING_DIR_HOST}${nonarch_base_libdir}/firmware"

EXTRA_OEMAKE += "${PACKAGECONFIG_CONFARGS} BINMAN_INDIRS=${BINMAN_INDIRS}"

PROVIDES += "u-boot"
PKG:${PN} = "u-boot"
PKG:${PN}-dev = "u-boot-dev"
PKG:${PN}-dbg = "u-boot-dbg"

S = "${WORKDIR}/git"

TOOLCHAIN = "gcc"
