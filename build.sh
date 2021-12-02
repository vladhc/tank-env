#!/bin/bash

set -e  # exit when any command fails

BUILD_TESTBED=false
BUILD_PYMODULE=false
BUILD_TESTS=false
BUILD_STRESS_TESTS=false

while test $# -gt 0
do
  case "$1" in
    --testbed) BUILD_TESTBED=true
        ;;
    --module) BUILD_PYMODULE=true
        ;;
    --tests) BUILD_TESTS=true
        ;;
    --stress) BUILD_STRESS_TESTS=true
        ;;
    --all)
        BUILD_TESTBED=true
        BUILD_PYMODULE=true
        BUILD_TESTS=true
        # DO NOT BUILD stress tests
        ;;
    --*) echo "bad option $1"
        ;;
    *) echo "argument $1"
        ;;
  esac
  shift
done

mkdir -p build
SRC_COMMON="src/geom.cc src/tank.cc src/env.cc src/lidar.cc"
SRC_COMMON="$SRC_COMMON src/game_object.cc src/bullet.cc"
SRC_COMMON="$SRC_COMMON src/strategic_point.cc src/collision_processor.cc"
INCLUDE_COMMON="-Iextern/box2d/include"

if $BUILD_TESTBED
then
  OUTPUT="build/testbed"
  rm -f $OUTPUT
  echo "Building Testbed"
  c++ $SRC_COMMON \
    src/keyboard_controller.cc \
    src/renderer.cc src/testbed.cc \
    -g -rdynamic \
    $INCLUDE_COMMON \
    -std=c++11 -w \
    -Llib \
    -lSDL2 \
    -lbox2d \
    -o $OUTPUT
  echo "Done"
fi

if $BUILD_TESTS
then
  OUTPUT="build/tanks-test"
  rm -f $OUTPUT
  echo "Building and running tests"
  c++ $SRC_COMMON \
    tests/env_test.cc tests/geom_test.cc tests/tank_test.cc \
    src/chunk.cc tests/chunk_test.cc tests/lidar_test.cc \
    -g -rdynamic \
    $INCLUDE_COMMON \
    -Isrc \
    -Iextern/googletest/googletest/include \
    -std=c++11 -w \
    -Llib \
    -lbox2d \
    -lgtest \
    -pthread \
    -o $OUTPUT
  LD_LIBRARY_PATH="$(pwd)/lib:$LD_LIBRARY_PATH" $OUTPUT
  echo "Done"
fi

if $BUILD_STRESS_TESTS
then
  OUTPUT="build/stress-test"
  rm -f $OUTPUT
  echo "Building and running stress tests"
  c++ $SRC_COMMON \
    tests/stress_test.cc \
    -g -rdynamic \
    $INCLUDE_COMMON \
    -Isrc \
    -Iextern/googletest/googletest/include \
    -std=c++11 -w \
    -Llib \
    -lbox2d \
    -lgtest \
    -pthread \
    -o $OUTPUT
  LD_LIBRARY_PATH="$(pwd)/lib:$LD_LIBRARY_PATH" $OUTPUT
  echo "Done"
fi

if $BUILD_PYMODULE
then
  OUTPUT="build/tanks$(python-config --extension-suffix)"
  rm -f "$OUTPUT"
  echo "Building python shared library"
  c++ \
    $SRC_COMMON \
    src/renderer.cc \
    src/module.cc src/chunk.cc \
    -O3 -Wall -shared -std=c++11 \
    -fPIC \
    $(python-config --includes) \
    -Iextern/pybind11/include \
    $INCLUDE_COMMON \
    -Llib \
    -lSDL2 \
    -lbox2d \
    -o "$OUTPUT"
  echo "Done"
fi
