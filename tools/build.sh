#!/bin/bash
# -----------------------------------------------------------------------
# build.sh - Builds PicoBoot firmware files for different platforms
# -----------------------------------------------------------------------
# Purpose: 
#   Builds PicoBoot firmware files for both Raspberry Pi Pico and Pico 2 
#   platforms. Processes gekkoboot.dol into payload files and creates 
#   universal payload that works on both boards.
#   Outputs uf2 files into dist/ directory.
#
# Usage: 
#   ./build.sh
# -----------------------------------------------------------------------

set -e

GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

platforms=("rp2040" "rp2350")
boards=("pico_w" "pico2_w")
boards_arch=("pico" "pico2")
families=("rp2040" "rp2350-arm-s")
build_type="RelWithDebInfo"

num_configs=${#platforms[@]}

if [ ! -f "payload.dol" ]; then
    echo -e "${RED}Error: payload.dol file not found${NC}"
    exit 1
fi

echo -e "${BLUE}##########################################################${NC}"
echo -e "🚀 ${YELLOW}Generating payload UF2 files:${NC}"
echo -e "📂 ${YELLOW}Input file:${NC} ${GREEN}payload.dol${NC}"
echo -e "${BLUE}##########################################################${NC}"

if [ ! -d "dist" ]; then
    mkdir dist
fi

echo -e "\n🔨 ${YELLOW}Building payload uf2 file for Pico...${NC}"
tools/process_ipl.py dist/payload_pico.uf2 payload.dol rp2040

echo -e "\n🔨 ${YELLOW}Building payload uf2 file for Pico 2...${NC}"
tools/process_ipl.py dist/payload_pico2.uf2 payload.dol rp2350

echo -e "\n🔨 ${YELLOW}Building universal payload UF2 file...${NC}"
tools/process_ipl.py dist/payload_universal.uf2 payload.dol

for (( i=0; i<num_configs; i++ )); do
        platform="${platforms[i]}"
        board="${boards[i]}"
        board_arch="${boards_arch[i]}"
        output_file="picoboot_full_${board_arch}.uf2"
        build_dir="${platform}_${board}"
        family="${families[i]}"

        echo -e "${BLUE}##########################################################${NC}"
        echo -e "🚀 ${YELLOW}Building for Platform:${NC} ${GREEN}${platform}${NC}, ${YELLOW}Board:${NC} ${GREEN}${board}${NC}"
        echo -e "📂 ${YELLOW}Build directory:${NC} ${GREEN}build/${build_dir}${NC}"
        echo -e "${BLUE}##########################################################${NC}"

        echo -e "\n🔨 ${YELLOW}Generating build files...${NC}"

        cmake -B "build/${build_dir}" -DCMAKE_BUILD_TYPE="${build_type}" -DPICO_BOARD="${board}" -S .

        echo -e "\n🔨 ${YELLOW}Building...${NC}"
        cmake --build "build/${build_dir}" --config "${build_type}"

        echo -e "\n🔨 ${YELLOW}Copying to dist...${NC}"
        cp build/${build_dir}/picoboot.uf2 dist/picoboot_${board_arch}.uf2

        if [ "${platform}" == "rp2350" ]; then
            echo -e "\n🔨 ${YELLOW}RP2350 workaround: Building core uf2 file...${NC}"
            picotool uf2 convert build/${build_dir}/dist/picoboot.bin build/${build_dir}/picoboot.uf2 --family rp2350-arm-s
        else
            echo -e "\n🔨 ${YELLOW}RP2040 workaround: Padding binary to 4k block size...${NC}"
            tools/rp2040_4k_align.sh build/${build_dir}/dist/picoboot.bin build/${build_dir}/dist/picoboot_padded.bin
            picotool uf2 convert build/${build_dir}/dist/picoboot_padded.bin build/${build_dir}/picoboot.uf2 --family rp2040
        fi

        echo -e "\n🔨 ${YELLOW}Merging binaries...${NC}"
        uf2tool join -o dist/${output_file} build/${build_dir}/picoboot.uf2 dist/payload_${board_arch}.uf2 --family ${family}

        echo -e "✨ ${GREEN}Build finished for ${platform} (${board})!${NC}\n"
done

echo -e "${BLUE}##########################################################${NC}"
echo -e "🎉 ${GREEN}All builds completed successfully!${NC}"
echo -e "${BLUE}##########################################################${NC}"
