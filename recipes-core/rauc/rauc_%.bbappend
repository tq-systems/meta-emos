FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

RAUC_KEYRING_FILE ?= "${@bb.utils.which(d.getVar('BBPATH'), 'files/emos/rauc/ca.cert.pem')}"
