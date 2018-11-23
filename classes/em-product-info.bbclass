DECSCRIPTION = "Product information"
LICENSE = "TQSSLA_V1.0.1"

PRODUCT_MANUFACTURER ?= "TQ-Systems GmbH"

# This is used for the default hostname
PRODUCT_CODE ?= "EM400"
PRODUCT_NAME ?= "Energy Manager 400"
PRODUCT_SWVERSION ?= "${DISTRO_VERSION}"

DEPENDS += "jq-native"

do_install() {
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
}
