# SPDX-License-Identifier: MIT
#
# Copyright (C) 2024 TQ-Systems GmbH <oss@ew.tq-group.com>, D-82229 Seefeld, Germany.
# Author: Matthias Schiffer

# U-Boot, ti-sci-fw, OP-TEE and TF-A licenses
LICENSE = "GPL-2.0-or-later & TI-TFL & BSD-2-Clause & BSD-3-Clause & MIT"

DESCRIPTION = "Combined eMMC bootloader image for TI K3 SoCs"

inherit deploy uboot-config

inherit extra-license-depends
EXTRA_LICENSE_DEPENDS = "virtual/bootloader"
EXTRA_LICENSE_MCDEPENDS = "mc:${MACHINE}:${MACHINE}-k3r5:virtual/bootloader"

TIBOOT3_PREFIX ?= "tiboot3.bin"

SPL_OFFSET_KB ??= ""
UBOOT_OFFSET_KB ??= ""
BOOTPART_SIZE_KB ??= ""

def put_file(output, filename, start, end):

    with open(filename, mode='rb') as input:
        data = input.read()

    max_len = end - start
    if len(data) > max_len:
        bb.error("Input file '%s' exceeds maximum size %d bytes" % (filename, max_len))

    output.seek(start)
    output.write(data)

do_compile[cleandirs] = "${B}"
python do_compile () {
    import glob

    spl_offset = 1024*int(d.getVar('SPL_OFFSET_KB'))
    uboot_offset = 1024*int(d.getVar('UBOOT_OFFSET_KB'))
    bootpart_size = 1024*int(d.getVar('BOOTPART_SIZE_KB'))

    k3r5_deploydir = d.getVar(f"DEPLOY_DIR_IMAGE_MC_{d.getVar('MACHINE')}-k3r5")
    prefix = f"{k3r5_deploydir}/{d.getVar('TIBOOT3_PREFIX')}"
    spl = d.expand('${DEPLOY_DIR_IMAGE}/${SPL_BINARY}')
    uboot = d.expand('${DEPLOY_DIR_IMAGE}/${UBOOT_BINARY}')

    # The code is not as flexible as it could be - we currently expect multiple
    # configs for the R5 stage, but only a single one for the A53 SPL / U-Boot.

    for tiboot3 in glob.glob(prefix + '*'):
        # Do not use removeprefix() to support build with Python 3.8
        config = tiboot3[len(prefix):]

        with open('bootloader%s.bin' % config, mode='wb') as output:
            put_file(output, tiboot3, 0, spl_offset)
            put_file(output, spl, spl_offset, uboot_offset)
            put_file(output, uboot, uboot_offset, bootpart_size)
}
do_compile[depends] += "virtual/bootloader:do_deploy"
do_compile[mcdepends] += "mc:${MACHINE}:${MACHINE}-k3r5:virtual/bootloader:do_deploy"

do_install () {
    install -D -m 644 -t ${D}/boot ${B}/bootloader*.bin
}
FILES:${PN} = "/boot"

do_deploy () {
    install -D -m 644 -t ${DEPLOYDIR} ${B}/bootloader*.bin
}
addtask deploy before do_build after do_compile

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "k3"
