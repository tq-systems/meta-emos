# Overwrite PREFIX in Makefile (PREFIX ?= /usr/local) for the correct installation path
# https://bbs.archlinux.org/viewtopic.php?id=176190
EXTRA_OEMAKE += "PREFIX=/usr"
