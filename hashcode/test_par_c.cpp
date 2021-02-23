#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <utility>

#include "par_c.h"

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

struct RealTimeManager {
  system_clock::time_point start_;

  RealTimeManager() : start_(system_clock::now()) {}

  double get_time()
  {
    auto t = system_clock::now();
    return duration_cast<milliseconds>(t - start_).count();
  }
};

int main()
{
  tanzaku::par_c::ThreadPoolRAII raii;

  std::cerr << __FUNCTION__ << " " << __LINE__ << std::endl;

  const int64_t n = 100000000;
  {
    RealTimeManager tm;

    std::vector<std::future<std::pair<double, int>>> futures;
    for (int i = 0; i < 16; i++) {
      auto future = tanzaku::par_c::enqueue([n, i] {
        auto res = std::make_pair(-1.0, 0);
        for (int j = n * i / 16; j < n * (i + 1) / 16; j++) {
          res = std::max(res, std::make_pair(std::sin(j), j));
        }
        return res;
      });

      futures.emplace_back(std::move(future));
    }

    auto res = std::make_pair(-1.0, 0);
    for (int i = 0; i < 16; i++) {
      res = std::max(res, futures[i].get());
    }

    std::cerr << __LINE__ << " " << res.first << " " << res.second << " "
              << tm.get_time() << std::endl;
  }

  {
    RealTimeManager tm;

    auto ranges = tanzaku::par_c::split(n);

    auto reducer = [](std::pair<double, int> lhs, std::pair<double, int> rhs) {
      return std::max(lhs, rhs);
    };

    auto mapper = [&](const tanzaku::par_c::Range &r) {
      auto res = std::make_pair(-1.0, 0);
      for (int i = r.from; i < r.to; i++) {
        res = reducer(res, std::make_pair(std::sin(i), i));
      }
      return res;
    };

    auto res = tanzaku::par_c::map_reduce(mapper, reducer, ranges);

    std::cerr << __LINE__ << " " << res.first << " " << res.second << " "
              << tm.get_time() << std::endl;
  }

  {
    RealTimeManager tm;

    std::vector<uint32_t> seeds(tanzaku::par_c::get_num_threads());
    std::iota(seeds.begin(), seeds.end(), 0);

    auto runner = [&](uint32_t seed) {
      for (int i = 0; i < 1000000000; i++) {
        seed = seed * 31 + 17;
      }

      std::cerr << __LINE__ << " " << seed << " " << tm.get_time() << std::endl;
    };

    tanzaku::par_c::run(runner, seeds);

    std::cerr << __LINE__ << " " << tm.get_time() << std::endl;
  }

  {
    RealTimeManager tm;

    auto res = std::make_pair(std::sin(0), 0);
    for (int i = 1; i < n; i++) {
      res = std::max(res, std::make_pair(std::sin(i), i));
    }

    std::cerr << __LINE__ << " " << res.first << " " << res.second << " "
              << tm.get_time() << std::endl;
  }

  {
    RealTimeManager tm;

    std::vector<std::thread> threads;
    std::vector<std::pair<double, int>> data(16);
    for (int thread_index = 0, start = 0; thread_index < 16; thread_index++) {
      int end = start + n / 16 + (thread_index < n % 16);

      threads.emplace_back([&data, thread_index, start, end]() {
        auto res = std::make_pair(std::sin(0), 0);
        for (int i = 1; i < n; i++) {
          res = std::max(res, std::make_pair(std::sin(i), i));
        }
        data[thread_index] = res;
      });

      start = end;
    }

    for (auto &thread : threads) {
      thread.join();
    }

    auto res = data[0];
    for (int i = 1; i < int(data.size()); i++) {
      res = std::max(res, data[i]);
    }

    std::cerr << __LINE__ << " " << res.first << " " << res.second << " "
              << tm.get_time() << std::endl;
  }

  //   parallelize::run([&]() {
  //     for (int it = 0; it < 100000000; it++) {
  //       ;
  //     }
  //   });
}
