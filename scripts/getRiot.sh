#!/bin/bash

sudo apt-get update
sudo apt-get install -y git dos2unix
sudo apt-get install -y gcc-arm-none-eabi
sudo apt-get install -y openocd

cd .. || exit

echo "Cloning RIOT repo..."
git clone "https://github.com/RIOT-OS/RIOT.git"

echo "Changing Makefile in robot project..."
pwd
file_path=$(pwd)/pzsp2_robot/Makefile
cd pzsp2_robot || exit
sed -i 's|RIOTBASE ?=  /data/RIOT/RIOT|#RIOTBASE ?= /data/RIOT/RIOT|' "$file_path"
sed -i 's|#RIOTBASE ?= $(CURDIR)/../../RIOT|RIOTBASE ?= $(CURDIR)/../RIOT|' "$file_path"
cd ..

echo "Changing file format to Unix in RIOT directory..."
find "$(pwd)/RIOT" -type f -exec dos2unix {} \;

echo "Building project..."
cd pzsp2_robot || exit
make
