# SPDX-License-Identifier: MIT
#
# Copyright (C) 2024 TQ-Systems GmbH <oss@ew.tq-group.com>, D-82229 Seefeld, Germany.
# Author: Matthias Schiffer

# U-Boot, firmware-imx-8m and TF-A licenses
LICENSE = "GPL-2.0-or-later & Proprietary & BSD-3-Clause & MIT"
LIC_FILES_CHKSUM = "file://${WORKDIR}/README;md5=37d06a0951d8d51f9e892f547957ae10"

DESCRIPTION = "Bootloader images for all hardware supported by the em-aarch64 machine"

PROVIDES = "virtual/bootloader"
RPROVIDES:${PN}-env= "u-boot-default-env"

inherit deploy extra-license-depends
EXTRA_LICENSE_MCDEPENDS = ""
EXTRA_LICENSE_MCDEPENDS:append:em-with-em4xx = " mc::em4xx:virtual/bootloader"
EXTRA_LICENSE_MCDEPENDS:append:em-with-eg4xx = " mc::eg4xx:virtual/bootloader"

SRC_URI = "\
    file://README \
    file://setup-u-boot-env \
"

do_compile[cleandirs] = "${B}"
do_compile () {
    :
}
do_compile:append:em-with-em4xx () {
    cp "${DEPLOY_DIR_IMAGE_MC_em4xx}/flash.bin-512m" bootloader-em4xx-512m.bin
    cp "${DEPLOY_DIR_IMAGE_MC_em4xx}/flash.bin-1g" bootloader-em4xx-1g.bin

    cp "${DEPLOY_DIR_IMAGE_MC_em4xx}/fw_env.config-em4xx" .
    # promote the 512m initial-env to the initial-env for all other em4xx configs, because they are identical anyways
    cp "${DEPLOY_DIR_IMAGE_MC_em4xx}/u-boot-initial-env-em4xx-512m" u-boot-initial-env-em4xx
}
do_compile:append:em-with-eg4xx () {
    cp "${DEPLOY_DIR_IMAGE_MC_eg4xx}/flash.bin-512m" bootloader-eg4xx-512m.bin

    cp "${DEPLOY_DIR_IMAGE_MC_eg4xx}/fw_env.config-eg4xx" .
    cp "${DEPLOY_DIR_IMAGE_MC_eg4xx}/u-boot-initial-env-eg4xx-512m" u-boot-initial-env-eg4xx
}

COMPILE_MCDEPENDS = ""
COMPILE_MCDEPENDS:append:em-with-em4xx = " mc::em4xx:virtual/bootloader:do_deploy"
COMPILE_MCDEPENDS:append:em-with-eg4xx = " mc::eg4xx:virtual/bootloader:do_deploy"
do_compile[mcdepends] += "${COMPILE_MCDEPENDS}"

INSTALL_CONFIGS = ""
INSTALL_CONFIGS:append:em-with-em4xx = " em4xx"
INSTALL_CONFIGS:append:em-with-eg4xx = " eg4xx"

do_install () {
    install -D -m 755 -t "${D}${base_sbindir}" "${WORKDIR}/setup-u-boot-env"

    for file in fw_env.config u-boot-initial-env; do
        for config in ${INSTALL_CONFIGS}; do
            install -D -m 644 -t "${D}${sysconfdir}" "${B}/${file}-${config}"
        done

        ln -s "/run/em/${file}" "${D}${sysconfdir}/${file}"
    done
}

PACKAGES += "${PN}-env"
FILES:${PN} = ""
ALLOW_EMPTY:${PN} = "1"
FILES:${PN}-env = "${sysconfdir}/ ${base_sbindir}/"

BOOTLOADERS:em-aarch64 = "bootloader-*.bin"
do_deploy () {
    install -D -m 644 -t "${DEPLOYDIR}" "${B}"/*
    cd "${DEPLOYDIR}" && \
    tar --owner=0 --group=0 --numeric-owner --sort=name --mtime=@0 \
        -chf "em-image-core-${MACHINE}-${DISTRO_VERSION}.bootloader.tar" \
        ${BOOTLOADERS}
    ln -sf "em-image-core-${MACHINE}-${DISTRO_VERSION}.bootloader.tar" "${DEPLOYDIR}/em-image-core-${MACHINE}.bootloader.tar"
}
addtask deploy before do_build after do_compile

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "em-aarch64"
