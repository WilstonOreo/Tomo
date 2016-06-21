#pragma once

#include <gex/algorithm/distance.hpp>

namespace tomo
{
  namespace routing
  {
    /// Empty StateIncrementer
    struct StateIncrementer
    {
      template<typename STATE>
      void operator()(const STATE& _prev, STATE& _next)
      {
        _next.time() += gex::distance(_prev,_next) / _next.speed();
      }
    };
  }
}

