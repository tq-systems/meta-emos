include linux-em.inc

PV = "5.4+git${SRCPV}"

SRCBRANCH = "em-fslc-5.4-1.0.0-imx"
SRCREV = "3fa677754ad3e29bdc226a8aa3ea005c5b2180ad"

COMPATIBLE_MACHINE = "^$"
COMPATIBLE_MACHINE:em = "mx8mn"
