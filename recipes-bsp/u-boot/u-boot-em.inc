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

# u-boot needs devtree compiler to parse dts files
DEPENDS += "dtc-native bc-native lzop-native flex-native bison-native python3-setuptools-native"

# u-boot needs these for binman
DEPENDS += "python3-pyelftools-native python3-pyyaml-native python3-jsonschema-native"

# SYSFW/TIFS Firmware
DEPENDS:append:k3 = " ti-sci-fw"
DEPENDS:append:k3r5 = " ti-sci-fw"

DM_FIRMWARE = "ipc_echo_testb_mcu1_0_release_strip.xer5f"

PLAT_SFX = ""
PLAT_SFX:am62xx = "am62xx"

PACKAGECONFIG[atf] = "BL31=${STAGING_DIR_HOST}/firmware/bl31.bin,,trusted-firmware-a"
PACKAGECONFIG[optee] = "TEE=${STAGING_DIR_HOST}${nonarch_base_libdir}/firmware/bl32.bin,,optee-os"
PACKAGECONFIG[dm] = "DM=${STAGING_DIR_HOST}${nonarch_base_libdir}/firmware/ti-dm/${PLAT_SFX}/${DM_FIRMWARE},,ti-dm-fw"

PACKAGECONFIG:append:aarch64 = " atf"
PACKAGECONFIG:append:am62xx = " dm"
PACKAGECONFIG:append:k3 = " optee"

BINMAN_INDIRS="${STAGING_DIR_HOST}${nonarch_base_libdir}/firmware"

EXTRA_OEMAKE += "${PACKAGECONFIG_CONFARGS} BINMAN_INDIRS=${BINMAN_INDIRS}"

PROVIDES += "u-boot"
PKG:${PN} = "u-boot"
PKG:${PN}-dev = "u-boot-dev"
PKG:${PN}-dbg = "u-boot-dbg"

S = "${WORKDIR}/git"

TOOLCHAIN = "gcc"
