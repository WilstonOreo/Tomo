#pragma once

#include <gex/algorithm/centroid.hpp>
#include "Orthogonal.hpp"

namespace tomo
{
  namespace slice
  {
    namespace functor
    {
      template<typename SLICE>
      struct Centroid {};
      
      template<gex::Axis AXIS>
      struct Centroid<Orthogonal<AXIS>>
      {
        typedef gex::Point3 output_type;

        template<typename SLICE>
        void operator()(const SLICE& _slice, output_type& _point)
        {
          using gex::algorithm::centroid;
          // Calc center point of slice polygon
          _point = gex::base::expand(centroid(_slice.polygon()),_slice.axis(),_slice.range().middle());
        }
      };
    }

    using functor::Centroid;

    template<typename SLICE, typename POINT>
    void centroid(const SLICE& _slice, POINT& _p)
    {
      Centroid<SLICE>()(_slice,_p);
    }

    template<typename SLICE>
    typename Centroid<SLICE>::output_type centroid(const SLICE& _slice)
    {
      typename Centroid<SLICE>::output_type _p;
      Centroid<SLICE>()(_slice,_p);
      return _p;
    }
  }
}
