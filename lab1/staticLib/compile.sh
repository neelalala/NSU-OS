#!/bin/bash

gcc -c static_lib.c -o static_lib.o
ar rc libstatic_hello.a static_lib.o
gcc hello.c -o hello -L. -lstatic_hello
