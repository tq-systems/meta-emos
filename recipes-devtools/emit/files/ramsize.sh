#!/bin/sh
# SPDX-License-Identifier: MIT

# Copyright (C) 2024 TQ-Systems GmbH <oss@ew.tq-group.com>, D-82229 Seefeld, Germany.
# Author: Matthias Schiffer
#
# Sums up the total sizes of all memory nodes found in /proc/device-tree
# and prints the result in MiB

# Allow 'local' feature supported by both busybox and dash
# shellcheck disable=SC3043

set -e

PREFIX='/proc/device-tree'

# Arguments:
#
# - Hexadecimal number without 0x prefix
hex2dec () {
	printf '%d' "0x$1"
}

# Arguments:
#
# - device-tree property to read
#
# Prints the property content as a newline-separated list list of 4-byte
# hexadecimal numbers (without 0x prefix)
readints () {
	hexdump -v -e '4/1 "%02x" "\n"' "$1"
}

# Arguments:
#
# - device-tree property to read
#
# Prints a string from the DTB up to the first newline or NUL byte
readstring () {
	tr <"$1" '\0' '\n' | head -n 1
}

# Arguments:
#
# - address-cells (decimal)
# - size-cells (decimal)
# - memory reg proparty DTB path
read_memory_node () {
	local address_cells="$1"
	local size_cells="$2"
	local prop="$3"

	local address_size_cells=$((address_cells + size_cells))

	# Number of hex digits to print
	# Cut off the last 5 hex digits of the size to convert from bytes
	# to MiB (implemented as string manipulation to avoid shell
	# arithmetic on large numbers)
	local print_len=$((8*size_cells - 5))

	local i=0
	local size=''

	for val in $(readints "$prop"); do
		i=$((i+1))

		if [ "$i" -le "$address_cells" ]; then
			continue
		fi
		size="${size}${val}"

		if [ "$i" -eq "$address_size_cells" ]; then
			printf "0x%.${print_len}s\n" "${size}"
			i=0
			size=''
		fi
	done
}


read_memory_nodes () {
	local address_cells size_cells total_size=0

	address_cells="$(hex2dec "$(readints "${PREFIX}/#address-cells" 2>/dev/null || echo '1')")"
	size_cells="$(hex2dec "$(readints "${PREFIX}/#size-cells" 2>/dev/null || echo '1')")"

	for node in "${PREFIX}/memory" "${PREFIX}/memory@"*; do
		if [ ! -e "${node}/reg" ]; then
			continue
		fi
		if [ -e "${node}/device_type" ] && [ "$(readstring "${node}/device_type")" != 'memory' ]; then
			continue
		fi

		for size in $(read_memory_node "$address_cells" "$size_cells" "${node}/reg"); do
			total_size=$((total_size + size))
		done
	done

	echo "$total_size"
}

read_memory_nodes
