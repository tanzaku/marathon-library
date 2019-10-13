#!/bin/bash

set -eu


### using gprof
# g++ JumpAround.cpp -pg -O3 -std=c++11 -Wall -Wextra -Wshadow -DADJUST
# java -jar tester.jar -exec "./a.out" -seed 2 -novis
# gprof a.out gmon.out > gmon.txt

### using gperftools
# brew install gperftools
# brew install graphviz

export CPUPROFILE=prof.out
export CPUPROFILE_FREQUENCY=100000
g++ JumpAround.cpp -O3 -std=c++11 -Wall -Wextra -Wshadow -DPROFILE -DADJUST -lprofiler -Wl,-no_pie
java -jar tester.jar -exec "./a.out" -seed 2 -novis
# otool -L ./a.out
pprof --text ./a.out $CPUPROFILE > prof.txt
pprof --dot ./a.out $CPUPROFILE > prof.dot && dot -T png prof.dot > prof.png
