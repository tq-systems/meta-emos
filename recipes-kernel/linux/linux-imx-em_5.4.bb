include linux-em.inc

PV = "5.4+git${SRCPV}"

SRCBRANCH = "em-fslc-5.4-1.0.0-imx"
SRCREV = "1671d9700e7a2e4668026e919110087951f2cc54"

COMPATIBLE_MACHINE = "^$"
COMPATIBLE_MACHINE:em = "mx8mn"
