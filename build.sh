#!/bin/bash

set -e  # exit when any command fails

EXT=$(python-config --extension-suffix)
rm -f tanks$EXT
rm -f render

echo "Building app for rendering"
c++ render.cc geom.cc tank.cc env.cc keyboard_controller.cc \
  -g -rdynamic \
  -Iextern/box2d/include \
  -std=c++11 -w \
  -lSDL2 \
  -lbox2d \
  -Lextern/box2d/build/bin \
  -o render

echo "Building shared library"
c++ -O3 -Wall -shared -std=c++11 \
  -fPIC \
  $(python-config --includes) \
  -o "tanks$EXT" \
  -Iextern/pybind11/include \
  -Iextern/box2d/include \
  env.cc geom.cc module.cc tank.cc

echo "Done"
