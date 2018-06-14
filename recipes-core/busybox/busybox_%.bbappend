FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
	    file://ash_cmdcmd.cfg \
	    file://telnetd.cfg \
	    file://devmem.cfg \
           "
