#pragma once

#include "for_each.hpp"
#include "centroid.hpp"
#include <gex/algorithm/intersects.hpp>

namespace tomo
{
  namespace slice
  {
    namespace strategy
    {
      namespace topology
      {
        struct Holes {};
        struct Slices {};
      }
    }

    namespace functor
    {
      template<typename STRATEGY>
      struct Topology
      {};

      template<>
      struct Topology<strategy::topology::Slices>
      {
        typedef gex::prim::MultiSegment<gex::Point3> output_type;

        template<typename GRAPH, typename STRATEGY, typename OUTPUT>
        void operator()(const GRAPH& _graph, STRATEGY _strategy, OUTPUT& _out)
        {
          typedef typename GRAPH::slice_type slice_type; 
          for_each(_graph,[&](const slice_type& _slice)
          {
            // Calc center point of slice polygon
            auto&& _a = centroid(_slice); 
            auto&& _upperSlices = _graph.indexesToPointers(_slice.upper());
            auto&& _lowerSlices = _graph.indexesToPointers(_slice.lower());
            for (auto& _connSlice : _upperSlices)
            {
              _out.emplace_back(_a,centroid(*_connSlice));
            }
            for (auto& _connSlice : _lowerSlices)
            {
              _out.emplace_back(_a,centroid(*_connSlice));
            }
          });
        }
      };

      template<>
      struct Topology<strategy::topology::Holes>
      {
        typedef gex::prim::MultiSegment<gex::Point3> output_type;
        
        template<typename GRAPH, typename STRATEGY, typename OUTPUT>
        void operator()(const GRAPH& _graph, STRATEGY _strategy, OUTPUT& _out)
        {
          typedef typename GRAPH::slice_type slice_type; 
          for_each(_graph,[&](const slice_type& _slice)
          {
            using namespace gex;
            using gex::base::expand;
            auto& _holes = _slice.polygon().holes();

            Scalar _middle = _slice.range().middle();
            Axis _axis = _slice.axis();

            for (auto& _hole : _holes)
            {
              // Calc center point of slice polygon
              auto&& _a = expand(algorithm::centroid(_hole),_axis,_middle);
              auto&& _upperSlices = _graph.indexesToPointers(_slice.upper());
              for (auto& _connSlice : _upperSlices)
              {
                auto& _upperHoles = _connSlice->polygon().holes();
                Scalar _upperMiddle = _connSlice->range().middle();
                
                for (auto& _upperHole : _upperHoles)
                {
                  if (!boost::geometry::intersects(_hole,_upperHole)) continue;

                  auto&& _centroid = expand(algorithm::centroid(_upperHole),_axis,_upperMiddle);
                  _out.emplace_back(_a,_centroid);
                }
              }
            }
          });
        }
      };
    }

    using functor::Topology;

    template<typename GRAPH, typename STRATEGY, typename OUTPUT>
    void topology(const GRAPH& _graph, STRATEGY _strategy, OUTPUT& _out)
    {
      Topology<STRATEGY>()(_graph,_strategy,_out);
    }
    
    template<typename GRAPH, typename STRATEGY>
    typename Topology<STRATEGY>::output_type 
    topology(const GRAPH& _graph, STRATEGY _strategy)
    {
      typename Topology<STRATEGY>::output_type _out; 
      topology(_graph,_strategy,_out);
      return _out;
    }
  }
}
