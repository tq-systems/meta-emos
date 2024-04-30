# SPDX-License-Identifier: MIT
#
# Copyright (C) 2024 TQ-Systems GmbH <oss@ew.tq-group.com>, D-82229 Seefeld, Germany.
# Author: Matthias Schiffer

# U-Boot, firmware-imx-8m, ti-sci-fw, OP-TEE and TF-A licenses
LICENSE = "GPL-2.0-or-later & Proprietary & TI-TFL & BSD-2-Clause & BSD-3-Clause & MIT"
LIC_FILES_CHKSUM = "file://${WORKDIR}/README;md5=2b18c23e5a347668fe60a6b3a3105c7b"

DESCRIPTION = "Bootloader images for all hardware supported by the em-aarch64 machine"

PROVIDES = "virtual/bootloader"

inherit deploy extra-license-depends
EXTRA_LICENSE_MCDEPENDS = "mc::em4xx:virtual/bootloader mc::em-cb30:k3-bootloader-image"

SRC_URI = "\
    file://README \
    file://setup-u-boot-env \
"

do_compile[cleandirs] = "${B}"
do_compile () {
    cp "${DEPLOY_DIR_IMAGE_MC_em4xx}/flash.bin" bootloader-em4xx.bin
    cp "${DEPLOY_DIR_IMAGE_MC_em-cb30}/bootloader-512m.bin" bootloader-em-cb30-512m.bin
    cp "${DEPLOY_DIR_IMAGE_MC_em-cb30}/bootloader-1g.bin" bootloader-em-cb30-1g.bin
    cp "${DEPLOY_DIR_IMAGE_MC_em-cb30}/bootloader-2g.bin" bootloader-em-cb30-2g.bin

    cp "${DEPLOY_DIR_IMAGE_MC_em4xx}/fw_env.config-em4xx" .
    cp "${DEPLOY_DIR_IMAGE_MC_em4xx}/u-boot-initial-env-em4xx" .
    cp "${DEPLOY_DIR_IMAGE_MC_em-cb30}/fw_env.config-em-cb30" .
    cp "${DEPLOY_DIR_IMAGE_MC_em-cb30}/u-boot-initial-env-em-cb30" .
}
do_compile[mcdepends] += "mc::em4xx:virtual/bootloader:do_deploy mc::em-cb30:k3-bootloader-image:do_deploy"

do_install () {
    install -D -m 755 -t "${D}${base_sbindir}" "${WORKDIR}/setup-u-boot-env"

    for file in fw_env.config u-boot-initial-env; do
        for config in em4xx em-cb30; do
            install -D -m 644 -t "${D}${sysconfdir}" "${B}/${file}-${config}"
        done

        ln -s "/run/em/${file}" "${D}${sysconfdir}/${file}"
    done
}

PACKAGES += "${PN}-env"
FILES:${PN} = ""
ALLOW_EMPTY:${PN} = "1"
FILES:${PN}-env = "${sysconfdir}/ ${base_sbindir}/"

do_deploy () {
    install -D -m 644 -t "${DEPLOYDIR}" "${B}"/*
}
addtask deploy before do_build after do_compile

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "em-aarch64"
