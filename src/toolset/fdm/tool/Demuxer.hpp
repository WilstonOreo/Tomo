#pragma once

#include <tomo/tool/Demuxer.hpp>
#include "Set.hpp"
#include "../item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    template<typename SOURCE, typename SINK>
    using Demuxer = tool::Demuxer<Toolset,SOURCE,SINK>;
  
    typedef Demuxer<item::SliceGraph,item::SliceGraph> SliceGraphDemuxer;
  }
}
