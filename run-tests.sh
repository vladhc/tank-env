#!/bin/bash

c++ \
  -Wall -std=c++11 \
  -o env-test \
  -Iextern/googletest/googletest/include \
  -Llib \
  -lgtest \
  -pthread \
  *_test.cc

LD_LIBRARY_PATH="$(pwd)/lib:$LD_LIBRARY_PATH" ./env-test
