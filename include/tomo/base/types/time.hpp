#pragma once

#include <chrono> 

namespace tomo
{
  namespace base
  {
    namespace types
    {
      typedef std::chrono::duration<double> Duration;
      typedef std::chrono::high_resolution_clock Time;
      typedef std::chrono::time_point<Time,Duration> TimePoint;
      typedef std::pair<TimePoint,TimePoint> Period;
    }
  }

  using base::types::TimePoint;
  using base::types::Period;
  using base::types::Duration;
  using base::types::Time;
}
