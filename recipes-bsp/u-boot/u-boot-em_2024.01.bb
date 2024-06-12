# SPDX-License-Identifier: MIT
#
# Copyright (C) 2024 TQ-Systems GmbH <oss@ew.tq-group.com>, D-82229 Seefeld, Germany.
# Author: Matthias Schiffer

require u-boot-em.inc

DESCRIPTION = "U-Boot for TQ-Systems Energy Manager"

LICENSE = "GPL-2.0-or-later"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=2ca5f2c35c8cc335f0a19756634782f1"

# ti-sci-fw, OP-TEE and TF-A licenses
LICENSE:append:k3 = " & TI-TFL & BSD-2-Clause & BSD-3-Clause & MIT"
# ti-sci-fw license
LICENSE:append:k3r5 = " & TI-TFL"
# TF-A and firmware-imx-8m licenses
LICENSE:append:mx8m = " & BSD-3-Clause & MIT & Proprietary"

inherit extra-license-depends
EXTRA_LICENSE_DEPENDS:k3 = "ti-sci-fw trusted-firmware-a optee-os"
EXTRA_LICENSE_DEPENDS:k3r5 = "ti-sci-fw"
EXTRA_LICENSE_DEPENDS:mx8m = "trusted-firmware-a firmware-imx-8m"

EXTRA_COMPILE_DEPENDS = ""
EXTRA_COMPILE_DEPENDS:mx8m = "firmware-imx-8m:do_deploy"

SRC_URI = " \
    git://github.com/tq-systems/u-boot-em.git;branch=${SRCBRANCH};protocol=https \
    file://fw_env.config \
"
SRC_URI:remove:k3r5 = " \
    file://fw_env.config \
"

SRCBRANCH = "em-v2024.01"
SRCREV = "4b20ef5dae16471e2e452285cfe754e9b2e91de5"

do_compile:prepend:mx8m() {
	if [ -n "${UBOOT_CONFIG}" ]; then
		for config in ${UBOOT_MACHINE}; do
			i=$(expr $i + 1);
			for type in ${UBOOT_CONFIG}; do
				j=$(expr $j + 1);
				if [ $j -eq $i ]; then
					for file in ${DDR_FIRMWARE_NAME}; do
						cp "${DEPLOY_DIR_IMAGE}/$file" "${B}/${config}/"
					done
				fi
			done
			unset j
		done
		unset i
	else
		for file in ${DDR_FIRMWARE_NAME}; do
			cp "${DEPLOY_DIR_IMAGE}/$file" "${B}/"
		done
	fi
}
do_compile[depends] += "${EXTRA_COMPILE_DEPENDS}"

do_deploy:append () {
    # If we have multiple configs, only deploy the individual variants with
    # -${type} suffix, and delete the "generic" symlinks to avoid confusion
    if [ -n "${UBOOT_CONFIG}" ]; then
        rm -f ${DEPLOYDIR}/${UBOOT_BINARY}
        rm -f ${DEPLOYDIR}/${UBOOT_SYMLINK}
        if [ -n "${SPL_BINARY}" ]; then
          rm -f ${DEPLOYDIR}/${SPL_BINARYFILE}
          rm -f ${DEPLOYDIR}/${SPL_SYMLINK}
        fi
    fi
}

UBOOT_INITIAL_ENV = "u-boot-initial-env"

COMPATIBLE_MACHINE = "^$"
COMPATIBLE_MACHINE:em310 = "^em310$"
COMPATIBLE_MACHINE:em4xx = "^em4xx$"
COMPATIBLE_MACHINE:imx8mn-egw = "^imx8mn-egw$"
COMPATIBLE_MACHINE:em-cb30 = "^em-cb30$"
COMPATIBLE_MACHINE:em-cb30-k3r5 = "^em-cb30-k3r5$"
