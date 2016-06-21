#pragma once

#include "tomo/misc.hpp"

namespace tomo
{
  namespace slice
  {
    template<typename GRAPH>
    struct Sink
    {
      typedef GRAPH graph_type;
      typedef typename graph_type::slice_type slice_type;
      typedef typename slice_type::tag_type tag_type;
      friend graph_type;

      Sink(graph_type& _graph, const tag_type& _tag) : 
        graph_(_graph),
        tag_(_tag)
      {
      }

      template<typename POLYGON, typename PATH>
      void insert(const POLYGON& _polygon, const PATH& _path)
      {
        graph_.insert(new slice_type(tag_,_polygon,_path));
      }

      TBD_PROPERTY_RO(tag_type,tag)
    private:
      graph_type& graph_;
    };
  }
}
