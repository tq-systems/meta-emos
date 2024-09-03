SUMMARY = "Linux kernel for TQ-Systems Energy Managers"

include linux-em.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

SRCBRANCH = "em-6.6.x"
SRCREV = "b125324c92a834aaafd2b619b53a0a64e0205859"

# LINUX_VERSION must match version from Makefile
LINUX_RELEASE = "6.6"
LINUX_VERSION = "${LINUX_RELEASE}.50"

COMPATIBLE_MACHINE = "^em$"
