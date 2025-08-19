#!/bin/bash

clang src/*.c -o Executable -Wall -Wextra -O2

if [ $? -eq 0 ]; then
    ./Executable
fi