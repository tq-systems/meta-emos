FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
	    file://devmem.cfg \
	    file://telnetd.cfg \
	    file://unxz.cfg \
           "
