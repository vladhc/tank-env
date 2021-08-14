#!/bin/bash

set -e  # exit when any command fails

EXT=$(python-config --extension-suffix)
rm -f tanks$EXT
rm -f render

echo "Building app for rendering"
c++ render.cc geom.cc tank.cc env.cc \
  -std=c++11 -w -lSDL2 -o render

echo "Building shared library"
c++ -O3 -Wall -shared -std=c++11 \
  -fPIC \
  $(python-config --includes) \
  -o "tanks$EXT" \
  -Iextern/pybind11/include \
  env.cc geom.cc module.cc tank.cc

echo "Done"
