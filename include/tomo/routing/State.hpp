#pragma once

#include <tbd/parameter.h>
#include "StateIncrementer.hpp"

namespace tomo
{
  namespace routing
  {
    template<typename MOVE>
    struct State : MOVE
    {
      typedef MOVE move_type;
      typedef double time_type;

      State(
          const move_type& _move = move_type(0,0,0),
          const time_type& _time = 0.0) :
        move_type(_move),
        time_(_time) 
      {}

      void operator()(const move_type& _move)
      {
        *this = _move;
      }

      TBD_PROPERTY_REF(time_type,time)
    };
  }
}
