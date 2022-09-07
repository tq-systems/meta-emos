FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Explicitly set RAUC_KEYRING_FILE here - Overrides of RAUC_KEYRING_FILE from
# local.conf/cmdline should affect the product-info package only
RAUC_KEYRING_FILE = "ca.cert.pem"

# deactivate unused features to reduce dependencies of rauc
PACKAGECONFIG[streaming] = "--enable-streaming=no"
PACKAGECONFIG[network]   = "--enable-network=no"

do_install:append() {
	# Remove configuration and keyring: we add this in our product-info package
	rm ${D}${sysconfdir}/rauc/system.conf
	rm ${D}${sysconfdir}/rauc/${RAUC_KEYRING_FILE}
}
