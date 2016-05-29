#!/bin/bash

rm CMakeCache.txt

mkdir build

mkdir build/release
cd build/release/
cmake -DCMAKE_BUILD_TYPE=Release ../../
cd ../../

mkdir build/debug
cd build/debug/
cmake -DCMAKE_BUILD_TYPE=Debug ../../
cd ../../
