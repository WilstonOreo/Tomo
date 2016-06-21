#pragma once

#include <tomo/tool/Muxer.hpp>
#include "Set.hpp"
#include "../item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    template<typename SOURCE, typename SINK>
    using Muxer = tool::Muxer<Toolset,SOURCE,SINK>;
  
    typedef Muxer<item::SliceGraph,item::SliceGraph> SliceGraphMuxer;
  }
}
