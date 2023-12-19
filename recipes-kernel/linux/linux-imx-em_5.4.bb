include linux-em.inc

PV = "5.4+git${SRCPV}"

SRCBRANCH = "em-fslc-5.4-1.0.0-imx"
SRCREV = "2c1d3510ec662819164ee8818db1c1ad7849a644"

COMPATIBLE_MACHINE = "^$"
COMPATIBLE_MACHINE:em = "mx8mn"
