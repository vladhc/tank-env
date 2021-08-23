#!/bin/bash

set -e  # exit when any command fails

mkdir -p build
FILENAME="build/env-test"
rm -f "$FILENAME"
c++ \
  -Wall -std=c++11 \
  -o "$FILENAME" \
  -Iextern/googletest/googletest/include \
  -Iextern/box2d/include \
  -Llib \
  -lgtest \
  -lbox2d \
  -pthread \
  src/*_test.cc src/env.cc src/tank.cc src/geom.cc

LD_LIBRARY_PATH="$(pwd)/lib:$LD_LIBRARY_PATH" $FILENAME
