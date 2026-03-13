SUMMARY = "Linux kernel for TQ-Systems Energy Managers"

include linux-em.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

SRCBRANCH = "em-6.18.x"
SRCREV = "cf567912e0d69d98e3a86cdf11f86c6b5907f7c1"

# LINUX_VERSION must match version from Makefile
LINUX_RELEASE = "6.18"
LINUX_VERSION = "${LINUX_RELEASE}.16"

COMPATIBLE_MACHINE = "^em$"
