DECSCRIPTION = "Product information"
LICENSE = "TQSSLA_V1.0.1"

PRODUCT_MANUFACTURER ?= "TQ-Systems GmbH"

# This is used for the default hostname
PRODUCT_CODE ?= "EM400"
PRODUCT_NAME ?= "Energy Manager 400"

DEPENDS += "jq-native"

do_install() {
	install -d ${D}${sysconfdir}

	jq -nc \
		--arg manufacturer '${PRODUCT_MANUFACTURER}' \
		--arg code '${PRODUCT_CODE}' \
		--arg name '${PRODUCT_NAME}' \
		'{
			manufacturer: $manufacturer,
			code: $code,
			name: $name,
		}' > ${D}${sysconfdir}/product-info.json
}
