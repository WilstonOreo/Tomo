#pragma once

#include <gex/prim.hpp>

namespace tomo
{
  namespace routing
  {
    template<typename MOVE>
    gex::Point2 moveToPoint(const MOVE& _move)
    {
      return gex::Point2(_move.x(),_move.y());
    }
    
    template<typename PATH>
    gex::Ring pathToRing(const PATH& _path)
    {
      using namespace gex;
      typedef typename PATH::point_type move_type;
      Ring _ring;
      for_each<move_type>(_path,[&](const move_type& _move)
      {
        _ring.push_back(moveToPoint(_move));
      });
      _ring.update();
      return _ring;
    }
  }
}
