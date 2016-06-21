#pragma once

#include "sampling/Sampler.hpp"

namespace tomo
{
  namespace slice
  {
    namespace sampling
    {
      template<
        typename GEOMETRY, 
        typename TRANSFORMATION, 
        typename SIMPLIFY,
        typename SAMPLES, 
        typename SLICEGRAPH>
      void sample(
          const GEOMETRY& _geometry, 
          const TRANSFORMATION& _transform,
          const SIMPLIFY& _simplify,
          SAMPLES& _samples, 
          SLICEGRAPH& _sliceGraph)
      {
        Sampler<GEOMETRY,typename SAMPLES::value_type,typename SLICEGRAPH::slice_type>()
          (_geometry,_transform,_simplify,_samples,_sliceGraph);
      }
    }
  }
}
