#pragma once

namespace tomo
{
  namespace routing
  {
    namespace functor
    {
      template<typename PATH, typename STRATEGY>
      struct SpeedOptimization
      {
      };
    }
        
    template<typename PATH, typename STRATEGY>
    void optimize_speed(const PATH& _path, STRATEGY _strategy, PATH& _opt)
    {
      functor::SpeedOptimization<PATH,STRATEGY>()(_path,_strategy,_opt);
    }

    template<typename PATH, typename STRATEGY>
    PATH optimize_speed(const PATH& _path, STRATEGY _strategy)
    {
      PATH _opt;
      optimize_speed(_path,_strategy,_opt);
      return _opt;
    }
  }
}

#include "speed/Curvature.hpp"
#include "speed/Topology.hpp"

