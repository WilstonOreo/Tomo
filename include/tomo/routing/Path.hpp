#pragma once

#include <gex/prim/LineString.hpp>

namespace tomo
{
  namespace routing
  {
    template<typename MOVE>
    using Path = gex::prim::LineString<MOVE>;
  }
}
