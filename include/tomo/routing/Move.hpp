#pragma once

#include <tbd/property.h>
#include "tomo/base/types/Speed.hpp"

namespace tomo
{
  namespace routing
  {
    /// Move concept
    template<typename POS>
    struct Move : POS
    {
      typedef POS pos_type;
      typedef Speed speed_type;

      Move() : speed_(0) {}
      Move(const pos_type& _pos, const speed_type& _speed) : 
        pos_type(_pos), 
        speed_(_speed) {}

      TBD_PROPERTY_REF(speed_type,speed)
    };
  }
}
