#!/bin/bash

set -e

GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

platforms=("rp2040" "rp2350")
boards=("pico_w" "pico2_w")
boards_arch=("pico" "pico2")
families=("pico_w" "pico2_w")
output_files=("picoboot_full_pico.uf2" "picoboot_full_pico2.uf2")
build_type="RelWithDebInfo"

num_configs=${#platforms[@]}

if [ ! -f "gekkoboot.dol" ]; then
    echo -e "${RED}Error: gekkoboot.dol file not found${NC}"
    exit 1
fi

echo -e "${BLUE}##########################################################${NC}"
echo -e "🚀 ${YELLOW}Generating payload uf2 files:${NC}"
echo -e "📂 ${YELLOW}Build directory:${NC} ${GREEN}build/gekkoboot${NC}"
echo -e "${BLUE}##########################################################${NC}"

if [ ! -d "dist" ]; then
    mkdir dist
fi

echo -e "\n🔨 ${YELLOW}Building payload uf2 file for Pico...${NC}"
./process_ipl.py dist/payload_pico.uf2 gekkoboot.dol rp2040

echo -e "\n🔨 ${YELLOW}Building payload uf2 file for Pico 2...${NC}"
./process_ipl.py dist/payload_pico2.uf2 gekkoboot.dol rp2350

echo -e "\n🔨 ${YELLOW}Building universal payload uf2 file...${NC}"
cat dist/payload_pico.uf2 dist/payload_pico2.uf2 > dist/payload_universal.uf2

for (( i=0; i<num_configs; i++ )); do
        platform="${platforms[i]}"
        board="${boards[i]}"
        board_arch="${boards_arch[i]}"
        output_file="${output_files[i]}"
        build_dir="${platform}_${board}"

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

        echo -e "\n🔨 ${YELLOW}Merging binaries...${NC}"

        uf2tool join -o dist/${output_file} build/${build_dir}/picoboot.uf2 dist/payload_${board_arch}.uf2 --family ${platform}

        echo -e "✨ ${GREEN}Build finished for ${platform} (${board})!${NC}\n"
done

echo -e "${BLUE}##########################################################${NC}"
echo -e "🎉 ${GREEN}All builds completed successfully!${NC}"
echo -e "${BLUE}##########################################################${NC}"
