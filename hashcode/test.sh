#!/bin/bash

set -eu

# g++ main.cpp -o main.out -g -std=c++11 -fsanitize=address -fsanitize=undefined -Wall -Wextra -Wshadow -DADJUST -Iinclude
# g++ test_par_c.cpp -O3 -pthread -g -std=c++14 -fsanitize=address -fsanitize=undefined -Wall -Wextra -Wshadow -DADJUST -Iinclude
# ./a.out


# g++ test_fast_set.cpp -O3 -pthread -g -std=c++14 -fsanitize=address -fsanitize=undefined -Wall -Wextra -Wshadow -DADJUST -Iinclude
# ./a.out

g++ test_sparse_bitset.cpp -O3 -pthread -g -std=c++20 -fsanitize=address -fsanitize=undefined -Wall -Wextra -Wshadow -DADJUST -Iinclude
./a.out

