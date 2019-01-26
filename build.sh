#!/usr/bin/env bash

g++ -O3 --std=c++11 -Wall -Wno-missing-braces \
  -I vendor/glfw/include \
  -I vendor/imgui \
  -I vendor/glad/include \
  -L vendor/glfw/src \
  -L vendor/imgui \
  -L vendor/glad \
  -l glad \
  -l glfw3 \
  -l imgui \
  -l pthread \
  -framework Cocoa \
  -framework IOKit \
  -framework CoreVideo \
  -framework OpenGL \
  -o main \
  src/main.cpp
