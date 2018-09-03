FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
	    file://cryptsha.cfg \
	    file://devmem.cfg \
	    file://telnetd.cfg \
	    file://unxz.cfg \
           "
