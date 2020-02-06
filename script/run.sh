#!/bin/bash

set -eu

g++ JumpAround.cpp -O3 -std=c++11 -Wall -Wextra -Wshadow -DADJUST

# java -jar tester.jar -exec "./a.out" -seed 2 -delay 0 -sz 10

echo \|seed\|score\|
echo \|-\|-\|
for i in $(seq 1 10); do
    score=$(java -jar tester.jar -exec "./a.out" -seed ${i} -novis | grep "Score = " | cut -d " " -f 3)
    echo \|${i}\|${score}\|
done
