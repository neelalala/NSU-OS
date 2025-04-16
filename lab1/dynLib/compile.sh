#!/bin/bash

gcc -shared -fPIC dyn_lib.c -o libdynamic.so
gcc hello.c -o hello -L. -ldynamic
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
