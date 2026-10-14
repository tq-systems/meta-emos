#!/bin/bash
# Copyright (c) 2024 TQ-Systems GmbH

set -e

VERSION="$1"
FILE_DISTRO="conf/distro/emos.conf"

set_distro_version() {
	local version="$1"
	sed -i "s/^\(DISTRO_VERSION\s*=\s*\"\)[^\"]*\"/\1$version\"/" "$FILE_DISTRO"
}

set_distro_version "$VERSION"
git add "$FILE_DISTRO"
