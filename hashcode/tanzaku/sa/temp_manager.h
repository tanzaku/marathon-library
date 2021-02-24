#ifndef TEMP_MANAGER_H
#define TEMP_MANAGER_H

namespace tanzaku
{
namespace sa
{
namespace temp_manager
{
struct TemperatureManager {
  double start_temp_;
  double end_temp_;

  TemperatureManager(double start_temp, double end_temp)
      : start_temp_(start_temp),
        end_temp_(end_temp) {}

  // 0 <= time <= 1
  double get_temp(double time) const
  {
    return time * (end_temp_ - start_temp_) + start_temp_;
  }
};
} // namespace temp_manager
} // namespace sa
} // namespace tanzaku

#endif
