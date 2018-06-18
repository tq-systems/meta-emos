FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
	    file://telnetd.cfg \
	    file://devmem.cfg \
           "
