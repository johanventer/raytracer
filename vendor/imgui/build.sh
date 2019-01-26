#!/usr/bin/env bash

g++ -O2 --std=c++11 -Wall -c \
  -D IMGUI_IMPL_OPENGL_LOADER_GLAD \
  -I ../glfw/include \
  -I ../glad/include \
  imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_impl_opengl3.cpp imgui_impl_glfw.cpp imgui_demo.cpp

ar rsv libimgui.a \
  imgui.o \
  imgui_draw.o \
  imgui_widgets.o \
  imgui_impl_opengl3.o \
  imgui_impl_glfw.o \
  imgui_demo.o 

ranlib libimgui.a