#!/bin/bash

EXT=$(python-config --extension-suffix)
rm *$EXT
c++ -O3 -Wall -shared -std=c++11 -fPIC $(python-config --includes) -Iextern/pybind11/include *.cc -o "tanks$EXT"
