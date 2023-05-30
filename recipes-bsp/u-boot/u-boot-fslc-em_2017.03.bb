require recipes-bsp/u-boot/u-boot.inc

inherit fsl-u-boot-localversion pythonnative

DESCRIPTION = "U-Boot based on mainline U-Boot used by FSL Community BSP in \
order to provide support for some backported features and fixes, or because it \
was submitted for revision and it takes some time to become part of a stable \
version, or because it is not applicable for upstreaming."

LICENSE = "GPL-2.0-or-later"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=a2c678cfd4a4d97135585cad908541c6"

SRC_URI = " \
	git://github.com/tq-systems/u-boot-em.git;branch=${SRCBRANCH};protocol=https \
	file://fw_env.config \
"

SRCREV = "41b5af0683f14e1d88e1878ca599393d6f071244"
SRCBRANCH = "EM3x0-v2017.03+fslc"

PV = "v2017.03+git${SRCPV}"
LOCALVERSION = "-em"

S = "${WORKDIR}/git"

DEPENDS += "dtc-native"

PROVIDES += "u-boot u-boot-em"
RPROVIDES:${PN} += "u-boot u-boot-em"

UBOOT_INITIAL_ENV = "u-boot-initial-env"

EXTRA_OEMAKE += ' \
	HOSTCC="${BUILD_CC} ${BUILD_CPPFLAGS}" \
	HOSTLDFLAGS="${BUILD_LDFLAGS}" \
	HOSTSTRIP=true \
	PYTHON=nativepython \
'

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "^$"
COMPATIBLE_MACHINE:em = "mx28"
