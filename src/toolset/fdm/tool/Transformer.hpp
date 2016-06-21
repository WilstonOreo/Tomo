#pragma once

#include <tomo/tool/Transformer.hpp>
#include "Set.hpp"
#include "../item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    template<typename SOURCE, typename SINK>
    using Transformer = tool::Transformer<Toolset,SOURCE,SINK>;
    
    typedef Transformer<item::SliceGraph,item::SliceGraph> SliceGraphTransformer;
  }
}
