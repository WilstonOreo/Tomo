#pragma once

#include <gex/algorithm/boost.hpp>

namespace tomo
{
  namespace slice
  {
    namespace sampling
    {
      struct SliceGenerator
      {
        template<typename SAMPLE, typename POLYGONS, typename SLICEGRAPH>
        void operator()(const SAMPLE& _sample, 
                        POLYGONS& _polygons,
                        SLICEGRAPH& _sliceGraph)
        {
          typedef typename SLICEGRAPH::slice_type slice_type;
          for (auto& _polygon : _polygons)
          {
            gex::algorithm::correct(_polygon);
            _sliceGraph.insert(slice_type(_sample.range(),_polygon));
          }
        }
      };
    }
  }
}
