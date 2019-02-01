DECSCRIPTION = "Product information"
LICENSE = "TQSSLA_V1.0.2"
PACKAGE_ARCH = "${MACHINE_ARCH}"

PRODUCT_MANUFACTURER ?= "TQ-Systems GmbH"

# This is used for the default hostname
PRODUCT_CODE ?= "EM400"
PRODUCT_NAME ?= "Energy Manager 400"
PRODUCT_SWVERSION ?= "${DISTRO_VERSION}"

DEPENDS += "jq-native"

LIC_FILES_CHKSUM = "file://${@bb.utils.which(d.getVar('BBPATH'), 'files/custom-licenses/TQSSLA_V1.0.2')};md5=5a77156d011829e57ffe26e62f07ff2d"

SRC_URI = " \
	file://${RAUC_KEYRING_FILE} \
	"

RAUC_CONFIG() {
[system]
compatible=${IMAGE_COMPATIBLE}
bootloader=uboot

[keyring]
path=/etc/rauc/ca.cert.pem
use-bundle-signing-time=true

[slot.u-boot.0]
device=/dev/mmcblk0
type=raw

[slot.rootfs.0]
device=/dev/mmcblk0p2
type=ext4
bootname=1

[slot.rootfs.1]
device=/dev/mmcblk0p3
type=ext4
bootname=2
}

python do_configure() {
    rauc_confdata = d.getVar('RAUC_CONFIG')
    rauc_conffile = d.expand('${WORKDIR}/system.conf')

    with open(rauc_conffile, 'w') as f:
        f.write(rauc_confdata)
}

do_install() {
	if [ '${RAUC_KEYRING_FILE}' = '${RAUC_EXAMPLE_KEYRING_FILE}' ]; then
		bbwarn "Using example RAUC key file '${RAUC_EXAMPLE_KEYRING_FILE}'. Set RAUC_KEYRING_FILE for product release builds."
	fi

	install -d ${D}${sysconfdir}

	jq -nc \
		--arg manufacturer '${PRODUCT_MANUFACTURER}' \
		--arg code '${PRODUCT_CODE}' \
		--arg name '${PRODUCT_NAME}' \
		--arg software_version '${PRODUCT_SWVERSION}' \
		'{
			manufacturer: $manufacturer,
			code: $code,
			name: $name,
			software_version: $software_version,
		}' > ${D}${sysconfdir}/product-info.json

	install -d ${D}${sysconfdir}/rauc
	install -m644 ${WORKDIR}/system.conf ${D}${sysconfdir}/rauc/
	install -m644 ${WORKDIR}/${RAUC_KEYRING_FILE} ${D}${sysconfdir}/rauc/ca.cert.pem
}

FILES_${PN} += " ${datadir}/common-licenses/README"

do_install_append() {
    install -d ${D}${datadir}/common-licenses

    cat > ${D}${datadir}/common-licenses/README << 'EOF'
Meaning of files in the package directories:
* 'recipeinfo' shows the license information of the software package, which is
  maintained in the build instruction.
* 'generic_<name of license>' shows a generic version of the license for
  comparison, as provided by the Yocto project.
* All other files in the package directories contain relevant license
  information. This includes, but is not limited to, license files,
  header files and other text files.
EOF
}