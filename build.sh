#!/usr/bin/bash

cd "$(dirname "$0")"

# rm -rf build
mkdir -p build
cd build
cmake -DPICO_BOARD=explorer ..
make -j20
