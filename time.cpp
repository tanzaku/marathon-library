

/**
 * シンプルなchronoを用いたタイマー
 */
#include <chrono>
class Timer
{
  std::chrono::system_clock::time_point start;

public:
  Timer() { reset(); }
  void reset() { start = std::chrono::system_clock::now(); }

  inline double elapsed()
  {
    auto end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  }
};

/**
 * rdtscを用いた高速なタイマー
 * サーバーのCPU周波数がわからない、もしくはジャッジサーバーのインスタンスが変更になる可能性がある場合は使わない方がいい
 */
const double CPU_FREQ_TOPCODER = 1.0 / (24 * 100 * 1000);
const double CPU_FREQ = CPU_FREQ_TOPCODER;

static unsigned long long rdtsc()
{
  unsigned int low, high;
  __asm__ volatile("rdtsc"
                   : "=a"(low), "=d"(high));
  return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
}

class Timer
{
  long long start;
  long long cycle;

public:
  Timer() { start = rdtsc(); }
  void reset() { start = rdtsc(); }

  inline double elapsed()
  {
    cycle = rdtsc();
    return (cycle - start) * CPU_FREQ;
  }
};
