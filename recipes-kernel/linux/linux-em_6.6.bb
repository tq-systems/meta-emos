SUMMARY = "Linux kernel for TQ-Systems Energy Managers"

include linux-em.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

SRCBRANCH = "em-6.6.x"
SRCREV = "b2466e375fe5522ff2eb5c6d4afb1519e609e2c2"

# LINUX_VERSION must match version from Makefile
LINUX_RELEASE = "6.6"
LINUX_VERSION = "${LINUX_RELEASE}.28"

COMPATIBLE_MACHINE = "^em$"
