#include "Move.hpp"

namespace tomo
{
  namespace fdm
  {
    typedef routing::Path<Move> Path;
  }
}

BOOST_GEOMETRY_REGISTER_LINESTRING(tomo::fdm::Path)
