#!/bin/bash
# -----------------------------------------------------------------------
# rp2040_4k_align.sh - Aligns binary file size to 4KB multiples
# -----------------------------------------------------------------------
# Purpose: 
#   Pads binary files to 4KB block size to work around an issue with 
#   the RP2040 bootrom as described in the RP2040 datasheet errata section.
#
# Usage: 
#   ./rp2040_4k_align.sh input_file output_file
#
# Arguments:
#   $1 - Input file path
#   $2 - Output file path
#
# Example:
#   ./rp2040_4k_align.sh picoboot.bin picoboot_padded.bin
# -----------------------------------------------------------------------

dd if=/dev/zero of="$2" bs=1 count=$(( ($(stat -c%s "$1") + 4096 - 1) / 4096 * 4096 )) 2>/dev/null
dd if="$1" of="$2" bs=1 count=$(stat -c%s "$1") conv=notrunc 2>/dev/null