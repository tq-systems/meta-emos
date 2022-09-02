FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append:emos = " \
	file://0900-enable-periodic-fsck.patch \
"
