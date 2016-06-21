#pragma once

#include "for_each_layer.hpp"

namespace tomo
{
  namespace slice
  {
    namespace util
    {
      template<typename SLICEGRAPH>
      void copyLayerSkeleton(const SLICEGRAPH& _in, SLICEGRAPH& _out)
      {
        _out.clear();
        typedef typename SLICEGRAPH::layer_type layer_type;
        for_each_layer(_in,_out,[&](const layer_type& _layer)
        {
          _out.insert(layer_type(_layer.range()));
        });
      }
    }
  }
}
