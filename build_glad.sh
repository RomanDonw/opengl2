#!/bin/bash

declare -r INCLUDE=./include
declare -r LIB=./lib
declare -r SRC=./lib/src

echo "Building GLAD..."

gcc -c -I"$INCLUDE" "$SRC/glad.c" -o "$LIB/glad.o"
ar rcs "$LIB/libglad.a" "$LIB/glad.o"

rm "$LIB/glad.o"
