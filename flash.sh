#!/usr/bin/bash

#cd "$(dirname "$0")"

#mkdir -p build
#mkdir -p bin
#cd build
#cmake ../
#make

cd "$(dirname "$0")"
cd build

#lsblk
#check if pico shows up as /dev/sda1
mount /dev/sda1 /mnt/pico
cp *.uf2 /mnt/pico
sync
umount /mnt/pico
