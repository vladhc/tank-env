#!/bin/bash

set -e  # exit when any command fails

c++ \
  -Wall -std=c++11 \
  -o env-test \
  -Iextern/googletest/googletest/include \
  -Llib \
  -lgtest \
  -pthread \
  *_test.cc env.cc tank.cc geom.cc

LD_LIBRARY_PATH="$(pwd)/lib:$LD_LIBRARY_PATH" ./env-test
