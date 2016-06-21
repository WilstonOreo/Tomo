#pragma once

#include "Graph.hpp"

namespace tomo
{
  namespace slice
  {
    namespace strategy
    {
      using gex::Scalar;

      struct Linear
      {
        Linear(
            Scalar _height,
            Scalar _layerHeight,
            Scalar _bottom) :
          height_(_height),
          layerHeight_(_layerHeight),
          bottom_(_bottom) {}

        TBD_PROPERTY_REF(Scalar,height)
        TBD_PROPERTY_REF(Scalar,layerHeight)
        TBD_PROPERTY_REF(Scalar,bottom)
      };
    }

    namespace functor
    {
      template<typename SLICE, typename STRATEGY>
      struct Extrusion {};

      template<typename SLICE>
      struct Extrusion<SLICE,strategy::Linear>
      {
        typedef SLICE slice_type;
        typedef Graph<slice_type> graph_type;

        template<typename MULTIPOLYGON, typename STRATEGY>
        void operator()(const MULTIPOLYGON& _multiPolygon, STRATEGY _strategy, graph_type& _output)
        {
          typedef typename slice_type::polygon_type polygon_type;

          for (gex::Scalar _pos = _strategy.bottom(); 
              _pos < _strategy.bottom() + _strategy.height(); 
              _pos += _strategy.layerHeight())
          {
            gex::Range _range(_pos,_pos + _strategy.layerHeight());
            for (auto& _polygon : _multiPolygon)
            {
              _output.insert(slice_type(_range,_polygon));
            }
            _output.connect();
          }
        }
      };
    }

    using functor::Extrusion;

    template<typename MULTIPOLYGON, typename STRATEGY, typename SLICE>
    void extrude(const MULTIPOLYGON& _multiPolygon, STRATEGY _strategy, Graph<SLICE>& _output)
    {
      Extrusion<SLICE,STRATEGY>()(_multiPolygon,_strategy,_output);
    }
  }
}
