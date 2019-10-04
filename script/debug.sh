#!/bin/bash

set -eu

g++ JumpAround.cpp -g -std=c++11 -fsanitize=address -fsanitize=undefined -Wall -Wextra -Wshadow -DADJUST
java -jar tester.jar -exec "./a.out" -seed 2 -delay 0 -sz 10 -manual
