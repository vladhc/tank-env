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

SRC_COMMON="geom.cc tank.cc env.cc  strategic_point.cc"
INCLUDE_COMMON="-Iextern/box2d/include"

if $BUILD_TESTBED
then
  rm -f render
  echo "Building Testbed"
  c++ $SRC_COMMON \
    keyboard_controller.cc \
    render.cc \
    -g -rdynamic \
    $INCLUDE_COMMON \
    -std=c++11 -w \
    -Llib \
    -lSDL2 \
    -lbox2d \
    -o render
  echo "Done"
fi

if $BUILD_PYMODULE
then
  FILENAME="tanks$(python-config --extension-suffix)"
  rm -f $FILENAME
  echo "Building python shared library"
  c++ \
    $SRC_COMMON \
    module.cc \
    -O3 -Wall -shared -std=c++11 \
    -fPIC \
    $(python-config --includes) \
    -Iextern/pybind11/include \
    $INCLUDE_COMMON \
    -Llib \
    -lbox2d \
    -o "$FILENAME"
  echo "Done"
fi
