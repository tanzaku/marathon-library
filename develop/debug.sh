#!/bin/bash

set -eu

g++ NN.cpp -g -std=c++11 -fsanitize=address -fsanitize=undefined -Wall -Wextra -Wshadow -DADJUST
./a.out
