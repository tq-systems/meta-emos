SUMMARY = "Linux kernel for TQ-Systems Energy Managers"

include linux-em.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

SRCBRANCH = "em-6.18.x"
SRCREV = "6273546ee0f9e05b4e8069770933095d9727b38e"

# LINUX_VERSION must match version from Makefile
LINUX_RELEASE = "6.18"
LINUX_VERSION = "${LINUX_RELEASE}.38"

COMPATIBLE_MACHINE = "^em$"
