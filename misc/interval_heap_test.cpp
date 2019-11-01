

#include "interval_heap.hpp"
#include <vector>

// http://natsugiri.hatenablog.com/entry/2016/10/10/035445
// とはmin/maxが逆

/**
 *            (0,1)
 *       (2,3)     (4,5)
 *    (6,7) (8,9)
 *
 *  to_min_l(i) -> 2*i+2,
 *  to_min_r(i) -> 2*i+4,
 *  to_max_l(i) -> 2*i+1,
 *  to_max_r(i) -> 2*i+3,
 *
 */

#include <algorithm>
#include <cstdint>
#include <cstdio>

struct XorShiftL {
  uint64_t x = 88172645463325252L;

  XorShiftL() {}

  XorShiftL(int seed) { x = seed; }

  uint64_t xor128() {
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
  }
} rng;

void verify() {
  interval_heap<int> heap;
  std::vector<int> data;
  for (int i = 0; i < 100000; i++) {
    if (data.empty() || rng.xor128() % 2 == 0) {
      int v = rng.xor128();
      // printf("put %d\n", v);
      heap.push(v);
      data.push_back(v);
    } else if (rng.xor128() & 1) {
      auto val = heap.peek_min();
      // printf("pop min %d\n", val);
      heap.pop_min();
      data.erase(data.begin());
    } else {
      auto val = heap.peek_max();
      // printf("pop max %d\n", val);
      heap.pop_max();
      data.pop_back();
    }
    // printf("done\n");
    sort(data.begin(), data.end());

    // printf("size %d %d\n", (int)heap.size(), (int)data.size());
    if (heap.size() != data.size()) {
      throw;
    }

    if (!heap.empty()) {
      // printf("min %d %d\n", (int)heap.peek_min(), (int)data.front());
      if (heap.peek_min() != data.front()) {
        throw;
      }
      // printf("max %d %d\n", (int)heap.peek_max(), (int)data.back());
      if (heap.peek_max() != data.back()) {
        throw;
      }
    }
  }
  puts("ok");
}

void speed() {
  interval_heap<int> heap;
  for (int i = 0; i < 10000000; i++) {
    if (heap.empty() || rng.xor128() % 10 <= 7) {
      int v = rng.xor128();
      heap.push(v);
    } else if (rng.xor128() & 1) {
      auto val = heap.peek_min();
      heap.pop_min();
    } else {
      auto val = heap.peek_max();
      heap.pop_max();
    }
  }
  puts("ok");
}

int main() {
  verify();
  // speed();
  return 0;
}
