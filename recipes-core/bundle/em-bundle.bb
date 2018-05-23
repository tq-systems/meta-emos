FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

inherit bundle

LICENSE="gateware"
LIC_FILES_CHKSUM = "file://hook.sh;beginline=3;endline=3;md5=5592c71186793d990aedc5988d4e19c2"

SRC_URI += "file://hook.sh"

RDEPENDS_${PN} += "imx28-blupdate"

RAUC_BUNDLE_COMPATIBLE ?= "em310"
RAUC_BUNDLE_SLOTS ?= "rootfs u-boot"
RAUC_BUNDLE_HOOKS[file] ?= "hook.sh"

RAUC_SLOT_rootfs ?= "emos-image"

RAUC_SLOT_u-boot ?= "u-boot"
RAUC_SLOT_u-boot[fstype] ?= "sb"
RAUC_SLOT_u-boot[hooks] ?= "install"

RAUC_KEY_FILE ?= "${@bb.utils.which(d.getVar('BBPATH'), 'files/emos/rauc/key.pem')}"
RAUC_CERT_FILE ?= "${@bb.utils.which(d.getVar('BBPATH'), 'files/emos/rauc/ca.cert.pem')}"
