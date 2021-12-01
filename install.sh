#!/bin/bash

set -e  # exit when any command fails
set -x  # show evaluation trace

git submodule update
sudo apt install cmake

ORIGIN_DIR=$(pwd)
LIB_DIR="$ORIGIN_DIR/lib"

mkdir -p "$LIB_DIR"

echo "Installing SDL GFX library and dependencies for other libs"
sudo apt install -y libsdl2-dev libsdl2-2.0-0 \
                    libjpeg-dev libwebp-dev libtiff5-dev libsdl2-image-dev libsdl2-image-2.0-0 \
                    libmikmod-dev libfishsound1-dev libsmpeg-dev liboggz2-dev libflac-dev libfluidsynth-dev libsdl2-mixer-dev libsdl2-mixer-2.0-0 \
                    libfreetype6-dev libsdl2-ttf-dev libsdl2-ttf-2.0-0 \
                    libx11-dev xorg-dev

echo "Installing Box2D..."

BOX2D_DIR="$ORIGIN_DIR/extern/box2d"
cd "$BOX2D_DIR"
# Copied from build.sh of the original repo.
# The difference is -DCMAKE_POSITION_INDEPENDENT_CODE=ON flag
rm -rf build
mkdir build
cd build
cmake -DBOX2D_BUILD_DOCS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DBOX2D_BUILD_UNIT_TESTS=OFF ..
cmake --build .

mv "$BOX2D_DIR/build/src/libbox2d.a" "$LIB_DIR"

echo "Installing GoogleTest library..."

GTEST_DIR="$ORIGIN_DIR/extern/googletest/googletest"
cd $GTEST_DIR

c++ -Wall -std=c++11 -fPIC \
  -Iinclude \
  -I. \
  -c \
  src/gtest-all.cc src/gtest_main.cc

c++ -Wall -shared -o libgtest.so gtest-all.o gtest_main.o
rm gtest-all.o
rm gtest_main.o

mv "$GTEST_DIR/libgtest.so" "$LIB_DIR"
