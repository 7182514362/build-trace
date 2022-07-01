#!/bin/bash

if [ -d "$PWD/build" ]; then
    rm -r build/*
else
    mkdir build
fi
cd build && cmake .. && make || exit 1
