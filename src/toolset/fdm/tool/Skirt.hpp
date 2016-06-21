#pragma once

#include "tomo/geometry/algorithm/RingOffset.hpp"

namespace tomo
{
  namespace fdm
  {
    struct Skirt
    {
      typedef geometry::prim::Ring Ring;
      typedef Ring::scalar_type scalar_type;

      Skirt(scalar_type _distance = 5, 
            scalar_type _width = 1,
            size_t _numRings = 3) : 
            distance_(_distance), 
            width_(_width),
            numRings_(_numRings) {}

      template<typename SLICES>
      std::vector<Ring> operator()(const SLICES& _slices)
      {
        using geometry::algorithm::RingOffset;
        using geometry::prim::MultiPolygon;
        using geometry::prim::Polygon;
        Ring _skirt;

        for (const auto& _slice : _slices)
        {
          /// Build convex hull for each slice
          Ring _hull;
          boost::geometry::convex_hull(_slice->polygons(),_hull);

          /// Unify it with hull of preceding slices 
          MultiPolygon _unionOutput;
          boost::geometry::union_(Polygon(_hull),Polygon(_skirt),_unionOutput);
          boost::geometry::convex_hull(_unionOutput,_skirt);
        }

        _skirt.update();

        /// Offset skirt with a certain width
        RingOffset<>()(_skirt,distance_);

        return Loops<>()(_skirt,width_,numRings_);
      }

      TBD_PROPERTY(scalar_type,distance)
      TBD_PROPERTY(scalar_type,width)
      TBD_PROPERTY(size_t,numRings)
    };
  }
}
