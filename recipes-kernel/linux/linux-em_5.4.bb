include linux-em.inc

PV = "5.4+git${SRCPV}"

SRCBRANCH = "em-5.4.x"
SRCREV = "9cc61691b54313bb599c0c746a5ebd972a474a07"

COMPATIBLE_MACHINE = "^$"
COMPATIBLE_MACHINE:em = "mx28"
