SUMMARY = "Linux kernel for TQ-Systems Energy Managers"

include linux-em.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

SRCBRANCH = "em-6.12.x"
SRCREV = "f0cb4379a7c119cc61116d2ed1b4704d02f98aa4"

# LINUX_VERSION must match version from Makefile
LINUX_RELEASE = "6.12"
LINUX_VERSION = "${LINUX_RELEASE}.26"

COMPATIBLE_MACHINE = "^em$"
