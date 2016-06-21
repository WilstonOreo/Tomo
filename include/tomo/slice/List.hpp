#pragma once

#include <vector>

namespace tomo
{
  namespace slice
  {
    template<typename SLICE>
    using ListMutable = std::vector<SLICE*>;

    template<typename SLICE>
    using ListConst = std::vector<SLICE const*>;

    template<typename SLICE, bool IS_CONST = false>
    struct ListType 
    {
      typedef ListMutable<SLICE> type;
    };

    template<typename SLICE>
    struct ListType<SLICE,true>
    {
      typedef ListConst<SLICE> type;
    };
  }
}
