#!/bin/bash

set -eu

g++ JumpAround.cpp -pg -O3 -std=c++11 -Wall -Wextra -Wshadow -DADJUST

java -jar tester.jar -exec "./a.out" -seed 2 -novis

gprof a.out gmon.out > gmon.txt
