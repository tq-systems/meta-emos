include linux-em.inc

PV = "5.4+git${SRCPV}"

SRCBRANCH = "em-fslc-5.4-1.0.0-imx"
SRCREV = "29147ce471184204a510370ccb34f82a18c1115f"

COMPATIBLE_MACHINE = "^$"
COMPATIBLE_MACHINE:em = "mx8mn"
