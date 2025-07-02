SUMMARY = "Linux kernel for TQ-Systems Energy Managers"

include linux-em.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

SRCBRANCH = "em-6.12.x"
SRCREV = "cb019691ed8b86b2b28c5d3eba4543e314816acc"

# LINUX_VERSION must match version from Makefile
LINUX_RELEASE = "6.12"
LINUX_VERSION = "${LINUX_RELEASE}.35"

COMPATIBLE_MACHINE = "^em$"
