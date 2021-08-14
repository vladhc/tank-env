#!/bin/bash

set -e  # exit when any command fails
set -x  # show evaluation trace

mkdir -p lib

echo "Installing GoogleTest library..."

GTEST_DIR="extern/googletest/googletest"
cd $GTEST_DIR

c++ -Wall -std=c++11 -fPIC \
  -Iinclude \
  -I. \
  -c \
  src/gtest-all.cc src/gtest_main.cc

c++ -Wall -shared -o libgtest.so gtest-all.o gtest_main.o
rm gtest-all.o
rm gtest_main.o

cd -
mv "$GTEST_DIR/libgtest.so" "lib/"

echo "Installing SDL GFX library"
sudo apt install -y libsdl2-dev libsdl2-2.0-0 \
                    libjpeg-dev libwebp-dev libtiff5-dev libsdl2-image-dev libsdl2-image-2.0-0 \
                    libmikmod-dev libfishsound1-dev libsmpeg-dev liboggz2-dev libflac-dev libfluidsynth-dev libsdl2-mixer-dev libsdl2-mixer-2.0-0 \
                    libfreetype6-dev libsdl2-ttf-dev libsdl2-ttf-2.0-0
