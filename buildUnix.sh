#!/bin/bash

mkdir build
cd build
cmake -S ../ -B .
make && make Shaders && ./VulkanGameEngine