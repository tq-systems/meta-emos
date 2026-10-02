SUMMARY = "Linux kernel for TQ-Systems Energy Managers"

include linux-em.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

SRCBRANCH = "em-6.18.x"
SRCREV = "30bcd1c94f6c86bf7220f800653c086e4b42b7be"

# LINUX_VERSION must match version from Makefile
LINUX_RELEASE = "6.18"
LINUX_VERSION = "${LINUX_RELEASE}.54"

COMPATIBLE_MACHINE = "^em$"
