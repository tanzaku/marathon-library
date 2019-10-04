#!/bin/bash

set -eu

g++ JumpAround.cpp -O3 -std=c++11 -Wall -Wextra -Wshadow -DADJUST

# java -jar tester.jar -exec "./a.out" -seed 2 -delay 0 -sz 10

for i in $(seq 1 100); do
    # java -jar tester.jar -exec "./a.out" -seed ${i} -novis | grep -E "Score = | recalc="
    java -jar tester.jar -exec "./a.out" -seed ${i} -novis | grep "Score = " | cut -d " " -f 3
    # java -jar tester.jar -exec "./a.out" -seed ${i} | grep "contests="
    # java -jar tester.jar -exec "./a.out" -seed ${i} -sz 10 -delay 1
done
