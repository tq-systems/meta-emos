FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
	file://cryptsha.cfg \
	file://devmem.cfg \
	file://telnetd.cfg \
	file://unxz.cfg \
	file://i2ctransfer.cfg \
"
