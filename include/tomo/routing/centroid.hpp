#pragma once

#include "for_each_move.hpp"
#include "num_moves.hpp"

namespace tomo
{
  namespace routing
  {
    namespace functor
    {
      template<typename PATHS>
      struct Centroid
      {
        template<typename POINT>
        void operator()(const PATHS& _paths, POINT& _centroid)
        {
          _centroid = POINT(0.0,0.0,0.0);
          typedef typename ForEachMove<PATHS>::move_type move_type;
          for_each_move(_paths,[&](const move_type& _move)
          {
            _centroid += POINT(_move);
          });

          size_t _numMoves = num_moves(_paths);

          _centroid.x() /= _numMoves;
          _centroid.y() /= _numMoves;
          _centroid.z() /= _numMoves;
        }
      };
    }

    template<typename PATHS, typename POINT>
    void centroid(const PATHS& _paths, POINT& _point)
    {
      functor::Centroid<PATHS>()(_paths,_point);
    }

    template<typename PATHS>
    gex::Point3 centroid(const PATHS& _paths)
    {
      gex::Point3 _centroid;
      centroid(_paths,_centroid);
      return _centroid;
    }
  }
}
