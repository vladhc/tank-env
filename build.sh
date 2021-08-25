#!/bin/bash

set -e  # exit when any command fails

BUILD_TESTBED=false
BUILD_PYMODULE=false

while test $# -gt 0
do
  case "$1" in
    --testbed) BUILD_TESTBED=true
        ;;
    --module) BUILD_PYMODULE=true
        ;;
    --*) echo "bad option $1"
        ;;
    *) echo "argument $1"
        ;;
  esac
  shift
done

mkdir -p build
SRC_COMMON="src/geom.cc src/tank.cc src/env.cc"
SRC_COMMON="$SRC_COMMON src/game_object.cc"
SRC_COMMON="$SRC_COMMON src/strategic_point.cc src/collision_processor.cc"
INCLUDE_COMMON="-Iextern/box2d/include"

if $BUILD_TESTBED
then
  OUTPUT="build/testbed"
  rm -f $OUTPUT
  echo "Building Testbed"
  c++ $SRC_COMMON \
    src/keyboard_controller.cc \
    src/render.cc \
    -g -rdynamic \
    $INCLUDE_COMMON \
    -std=c++11 -w \
    -Llib \
    -lSDL2 \
    -lbox2d \
    -o $OUTPUT
  echo "Done"
fi

if $BUILD_PYMODULE
then
  OUTPUT="build/tanks$(python-config --extension-suffix)"
  rm -f "$OUTPUT"
  echo "Building python shared library"
  c++ \
    $SRC_COMMON \
    src/module.cc \
    -O3 -Wall -shared -std=c++11 \
    -fPIC \
    $(python-config --includes) \
    -Iextern/pybind11/include \
    $INCLUDE_COMMON \
    -Llib \
    -lbox2d \
    -o "$OUTPUT"
  echo "Done"
fi
