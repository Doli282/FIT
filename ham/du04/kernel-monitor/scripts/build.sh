#/bin/bash

mkdir build
cd build
cmake ..
make
make module-clean
make module
