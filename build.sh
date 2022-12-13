#!/usr/bin/env bash

gcc run.c -o run
gcc run2.c -o run2
gcc run3.c -o run3
gcc run5.c -o run5
gcc -pthread -O3 run6.c -o run6