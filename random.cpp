
/**
 * https://ja.wikipedia.org/wiki/Xorshift
 * xor64の実装を参考にした
 */
#include <cmath>
#include <cstdint>
#include <tuple>
class XorShiftL
{
  static const int LOG_CACHE_SIZE = 0x10000;

  int logIndex = 0;
  double logCache[LOG_CACHE_SIZE];

  std::uint64_t x = 88172645463325252LL;

public:
  XorShiftL()
  {
    for (int i = 0; i < LOG_CACHE_SIZE; i++) {
      logCache[i] = log((i + 0.5) / 0x10000);
    }
    shuffle(logCache);
  }

  std::uint64_t xor64()
  {
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
  }

  // [0, n)
  int get(int n) { return xor64() % n; }

  // [low, high)
  int get(int low, int high) { return xor64() % (high - low) + low; }
  double getDouble() { return xor64() / double(std::numeric_limits<std::uint64_t>::max()); }

  /**
   * 2つの相異なる数をランダムに取得する
   * get<0>(result) < get<1>(result) が保証されている
   */
  std::tuple<int, int> get2(int low, int high)
  {
    int i = get(low, high);
    int j = get(low, high - 1);
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
  std::tuple<int, int> getUnordered2(int low, int high)
  {
    int i = get(low, high);
    int j = get(low, high - 1);
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
  void shuffle(T &s)
  {
    for (int i = 1; i < (int)s.size(); i++) {
      int j = get(0, i + 1);
      if (i != j) {
        std::swap(s[i], s[j]);
      }
    }
  }

  template <typename T, int n>
  void shuffle(T (&ary)[n])
  {
    for (int i = 1; i < n; i++) {
      int j = get(0, i + 1);
      if (i != j) {
        std::swap(ary[i], ary[j]);
      }
    }
  }
};
