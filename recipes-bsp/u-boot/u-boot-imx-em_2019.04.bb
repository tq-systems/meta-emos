require recipes-bsp/u-boot/u-boot.inc

inherit fsl-u-boot-localversion

DESCRIPTION = "U-Boot for TQ-Group EM4xx"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

SRC_URI = " \
	git://github.com/tq-systems/u-boot-em.git;branch=${SRCBRANCH};protocol=https \
	file://fw_env.config \
"

SRCREV = "0a027bd2eaaac52a8fbf976e66c3368123c4df4f"
SRCBRANCH = "EM4xx-v2019.04-tqmaxx"

PV = "v2019.04+git${SRCPV}"
LOCALVERSION = "-em"

S = "${WORKDIR}/git"

DEPENDS += "bison-native dtc-native"

IMX_EXTRA_FIRMWARE = ""
IMX_EXTRA_FIRMWARE_mx8m = "trusted-firmware-a firmware-imx-8m"

DEPENDS += "${IMX_EXTRA_FIRMWARE}"
do_compile[depends] += " \
    ${@' '.join('%s:do_deploy' % r for r in '${IMX_EXTRA_FIRMWARE}'.split() )} \
"

EXTRA_OEMAKE_append_mx8m = " ATF_LOAD_ADDR=0x960000"

PROVIDES += "u-boot u-boot-em"
RPROVIDES_${PN} += "u-boot u-boot-em"

UBOOT_INITIAL_ENV = "u-boot-initial-env"

do_compile_prepend_mx8m() {
    for file in ${DDR_FIRMWARE_NAME}; do
        cp "${DEPLOY_DIR_IMAGE}/$file" "${S}/"
    done

    if [ -n "${UBOOT_CONFIG}" ]; then
        for config in ${UBOOT_MACHINE}; do
            i=$(expr $i + 1);
            for type in ${UBOOT_CONFIG}; do
                j=$(expr $j + 1);
                if [ $j -eq $i ]; then
                    cp "${DEPLOY_DIR_IMAGE}/bl31.bin" "${B}/${config}/"
                fi
            done
            unset  j
        done
        unset  i
    else
        cp "${DEPLOY_DIR_IMAGE}/bl31.bin" "${B}/"
    fi
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "em4xx"
