#ifndef RANDOM_H
#define RANDOM_H

/**
 * https://ja.wikipedia.org/wiki/Xorshift
 * xor64の実装を参考にした
 */
#include <cmath>
#include <cstdint>
#include <tuple>

namespace tanzaku
{
namespace rng
{

static const int LOG_CACHE_SIZE = 0x10000;
double logCache[LOG_CACHE_SIZE];

class Random
{
  int logIndex;
  std::uint64_t x;

public:
  void init()
  {
    logIndex = 0;
    x = 88172645463325252ULL;

    for (int i = 0; i < LOG_CACHE_SIZE; i++) {
      logCache[i] = log((i + 0.5) / 0x10000);
    }
    shuffle(logCache);
  }

  // インチキsplit
  Random split()
  {
    Random rng;
    rng.logIndex = nextInt(LOG_CACHE_SIZE);
    rng.x = xor64();
    return rng;
  }

  std::uint64_t xor64()
  {
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
  }

  std::uint64_t xor64()
  {
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
  }

  // [0, n)
  int nextInt(int n) { return xor64() % n; }

  // [low, high)
  int nextInt(int low, int high) { return xor64() % (high - low) + low; }
  // double nextDouble() { return xor64() / double(std::numeric_limits<std::uint64_t>::max()); }

  /**
   * 2つの相異なる数をランダムに取得する
   * get<0>(result) < get<1>(result) が保証されている
   */
  std::tuple<int, int> nextIntPair(int low, int high)
  {
    int i = nextInt(low, high);
    int j = nextInt(low, high - 1);
    if (j >= i)
      j++;
    else
      std::swap(i, j);
    return std::make_tuple(i, j);
  }

  /**
   * 2つの相異なる数をランダムに取得する
   * get<0>(result) < get<1>(result) が保証されている
   */
  std::tuple<int, int> nextIntPair(int n)
  {
    int i = nextInt(n);
    int j = nextInt(n - 1);
    if (j >= i)
      j++;
    else
      std::swap(i, j);
    return std::make_tuple(i, j);
  }

  /**
   * 2つの相異なる数をランダムに取得する
   * get<0>(result) < get<1>(result) は保証されない
   */
  std::tuple<int, int> nextIntUnorderedPair(int low, int high)
  {
    int i = nextInt(low, high);
    int j = nextInt(low, high - 1);
    if (j >= i)
      j++;
    return std::make_tuple(i, j);
  }

  /**
   * 2つの相異なる数をランダムに取得する
   * get<0>(result) < get<1>(result) は保証されない
   */
  std::tuple<int, int> nextIntUnorderedPair(int n)
  {
    int i = nextInt(n);
    int j = nextInt(n - 1);
    if (j >= i)
      j++;
    return std::make_tuple(i, j);
  }

  /**
   * ログを取得する
   */
  double nextLog()
  {
    const double res = logCache[logIndex++];
    if (logIndex == LOG_CACHE_SIZE) {
      logIndex = 0;
    }
    return res;
  }

  template <typename T>
  T &get(T &s)
  {
    return s[nextInt(s.size())];
  }

  template <typename T>
  void shuffle(T &s)
  {
    for (int i = 1; i < (int)s.size(); i++) {
      int j = nextInt(0, i + 1);
      if (i != j) {
        std::swap(s[i], s[j]);
      }
    }
  }

  template <typename T, int n>
  void shuffle(T (&ary)[n])
  {
    for (int i = 1; i < n; i++) {
      int j = nextInt(0, i + 1);
      if (i != j) {
        std::swap(ary[i], ary[j]);
      }
    }
  }
};

} // namespace rng
} // namespace tanzaku

#endif