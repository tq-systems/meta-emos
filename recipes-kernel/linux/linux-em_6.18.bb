SUMMARY = "Linux kernel for TQ-Systems Energy Managers"

include linux-em.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

SRCBRANCH = "em-6.18.x"
SRCREV = "db01acc6da3493341cde374c7390b96adff2015a"

# LINUX_VERSION must match version from Makefile
LINUX_RELEASE = "6.18"
LINUX_VERSION = "${LINUX_RELEASE}.63"

COMPATIBLE_MACHINE = "^em$"
