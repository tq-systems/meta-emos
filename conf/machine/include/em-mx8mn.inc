# SPDX-License-Identifier: MIT
#
# Copyright (C) 2024 TQ-Systems GmbH <oss@ew.tq-group.com>, D-82229 Seefeld, Germany.
# Author: Matthias Schiffer
#
# Common definitions for meta-emos machines with i.MX8MN SoC


MACHINEOVERRIDES =. "mx8:mx8m:mx8mn:em:"

require conf/machine/include/arm/arch-arm64.inc

PREFERRED_PROVIDER_u-boot = "u-boot-em"
PREFERRED_PROVIDER_virtual/bootloader = "u-boot-em"

# Set DDR FIRMWARE
DDR_FIRMWARE_NAME = "lpddr4_pmu_train_1d_imem.bin lpddr4_pmu_train_1d_dmem.bin lpddr4_pmu_train_2d_imem.bin lpddr4_pmu_train_2d_dmem.bin"

UBOOT_SUFFIX = "bin"

# Strictly speaking, this isn't (just) the SPL, but we can get the
# U-Boot recipe to deploy the correct files without having to extend
# do_install/do_deploy this way.
SPL_BINARY = "flash.bin"

UBOOT_MAKE_TARGET = "${UBOOT_BINARY} ${SPL_BINARY}"

TFA_PLATFORM = "imx8mn"
TFA_BUILD_TARGET = "bl31"
COMPATIBLE_MACHINE:pn-trusted-firmware-a = "^mx8mn$"
