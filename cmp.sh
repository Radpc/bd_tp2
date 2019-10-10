#!/bin/bash

g++ -o tp2 src/main.cpp src/helper.cpp -pthread
printf "Compiled safely!\n"
printf "Run ./tp1 -i [filename(s)] to create the database.\n"
#./tp1
