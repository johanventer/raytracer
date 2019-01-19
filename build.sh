#!/usr/bin/env bash

g++ -O2 --std=c++11 -Wall -Wno-missing-braces -I vendor/glfw/include -L vendor/glfw/src -l glfw3 -l pthread -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -o main src/main.cpp
