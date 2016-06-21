#pragma once

#include <iostream>
#include "Move.hpp"
#include "GCode.hpp"
#include "State.hpp"

namespace tomo
{
  namespace fdm
  {
    typedef tomo::routing::Path<Move> Path;
    typedef tomo::routing::Route<Path,State> Route;
  }
}
