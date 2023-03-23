include linux-em.inc

PV = "5.4+git${SRCPV}"

SRCBRANCH = "em-fslc-5.4-1.0.0-imx"
SRCREV = "48fbb88f38d9d9c5c80959ce134df809afaf4d2f"

COMPATIBLE_MACHINE = "^$"
COMPATIBLE_MACHINE:em = "mx8mn"
