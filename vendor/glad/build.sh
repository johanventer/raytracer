#!/usr/bin/env bash

gcc -O2 -Wall -c \
  -I include \
  src/glad.c

ar rsv libglad.a glad.o
ranlib libglad.a