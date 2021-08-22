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

if $BUILD_TESTBED
then
  rm -f render
  echo "Building Testbed"
  c++ render.cc geom.cc tank.cc env.cc keyboard_controller.cc \
    strategic_point.cc \
    -g -rdynamic \
    -Iextern/box2d/include \
    -std=c++11 -w \
    -lSDL2 \
    -lbox2d \
    -Lextern/box2d/build/bin \
    -o render
  echo "Done"
fi

if $BUILD_PYMODULE
then
  EXT=$(python-config --extension-suffix)
  rm -f tanks$EXT
  echo "Building python shared library"
  c++ -O3 -Wall -shared -std=c++11 \
    -fPIC \
    $(python-config --includes) \
    -o "tanks$EXT" \
    -Iextern/pybind11/include \
    -Iextern/box2d/include \
    env.cc geom.cc module.cc tank.cc \
    strategic_point.cc
  echo "Done"
fi
