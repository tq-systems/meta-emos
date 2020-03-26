FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append_emos += " \
	file://0900-enable-periodic-fsck.patch \
"
