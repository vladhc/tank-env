#!/bin/bash

set -e  # exit when any command fails

c++ \
  -Wall -std=c++11 \
  -o env-test \
  -Iextern/googletest/googletest/include \
  -Iextern/box2d/include \
  -Llib \
  -Lextern/box2d/build/bin \
  -lgtest \
  -lbox2d \
  -pthread \
  *_test.cc env.cc tank.cc geom.cc

LD_LIBRARY_PATH="$(pwd)/lib:$LD_LIBRARY_PATH" ./env-test
