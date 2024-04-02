FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Explicitly set RAUC_KEYRING_FILE here - Overrides of RAUC_KEYRING_FILE from
# local.conf/cmdline should affect the product-info package only
RAUC_KEYRING_FILE = "ca.cert.pem"

SRC_URI += " \
	file://verified_message_as_gdebug.patch \
	file://10-rauc-emos.conf \
"

FILES_${PN} += "/etc/systemd/system/rauc.service.d"

do_install:append() {
	# Remove configuration and keyring: we add this in our product-info package
	rm ${D}${sysconfdir}/rauc/system.conf
	rm ${D}${sysconfdir}/rauc/${RAUC_KEYRING_FILE}

	install -D -m0644 ${WORKDIR}/10-rauc-emos.conf -t ${D}${sysconfdir}/systemd/system/rauc.service.d/
}
