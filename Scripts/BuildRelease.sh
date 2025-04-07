#!/bin/bash

BUILD_TYPE="Release"
BUILD_DIR="Release"

mkdir -p $BUILD_DIR
cmake -B $BUILD_DIR -S . -G Ninja -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build $BUILD_DIR --target chaos-in-world-generation -j $(nproc --all)