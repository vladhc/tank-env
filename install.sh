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
