#pragma once

#include "../item/SliceGraph.hpp"

#include <tomo/routing/duration.hpp>
#include <tomo/routing/average_speed.hpp>
#include <tomo/routing/length.hpp>

namespace tomo
{
  namespace fdm
  {
    namespace query
    {
      class Path : 
        public Serializer<Path>
      {
        TOMO_PARAMETER_LIST
        (
          (int) num_moves,
          (float) length,
          (float) duration,
          (float) average_speed
        )
      public:
        void operator()(const fdm::Route& _route)
        {
          length_ = routing::length(_route);
          duration_ = routing::duration(_route);
          average_speed_ = routing::average_speed(_route);
          num_moves_ = routing::num_moves(_route);
        }
      };

      class Paths :
        public Serializer<Paths>
      {
        TOMO_PARAMETER_LIST
        (
          (int) num_paths,
          (int) num_moves,
          (float) overall_length,
          (float) overall_duration,
          (float) average_speed
        )
      public:
        template<typename SLICEGRAPH>
        void operator()(const SLICEGRAPH& _sliceGraph)
        { 
          num_paths_ = routing::num_paths(_sliceGraph);
          overall_duration_ = routing::duration(_sliceGraph);
          overall_length_ = routing::length(_sliceGraph);
          average_speed_ = routing::average_speed(_sliceGraph);
          num_moves_ = routing::num_moves(_sliceGraph);
        }
      };
    }
  }
}
