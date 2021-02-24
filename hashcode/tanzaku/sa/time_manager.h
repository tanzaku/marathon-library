#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <chrono>
#include <cstdint>

namespace tanzaku
{
namespace sa
{
namespace time_manager
{

struct ConstantIterationTimeManager {
  std::int64_t iteration_count_;
  std::int64_t max_iteration_;

  ConstantIterationTimeManager(std::int64_t max_iteration)
      : iteration_count_(0),
        max_iteration_(max_iteration) {}

  double get_time()
  {
    iteration_count_ += 1;
    return iteration_count_ / double(max_iteration_);
  }
};

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

struct RealTimeManager {
  system_clock::time_point start_;
  double time_in_milli_;

  RealTimeManager(double time_in_milli)
      : start_(system_clock::now()),
        time_in_milli_(time_in_milli) {}

  double get_time()
  {
    auto t = system_clock::now();
    return duration_cast<milliseconds>(t - start_).count() / time_in_milli_;
  }
};

} // namespace time_manager
} // namespace sa
} // namespace tanzaku

#endif
