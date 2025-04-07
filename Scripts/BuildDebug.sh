#!/bin/bash

BUILD_TYPE="Debug"
DEBUG_BUILD_DIR="Debug"

mkdir -p $DEBUG_BUILD_DIR
cmake -B $DEBUG_BUILD_DIR -S . -G Ninja -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build $DEBUG_BUILD_DIR --target chaos-in-world-generation -j $(nproc --all)