#pragma once

#include <set>

namespace tomo
{
  namespace base
  {
    template<typename SLICE>
    using SetMutable = std::set<SLICE*>;

    template<typename SLICE>
    using SetConst = std::set<SLICE const*>;

    template<typename SLICE, bool IS_CONST = true>
    struct SetType 
    {
      typedef SetConst<SLICE> type;
    };

    template<typename SLICE>
    struct SetType<SLICE,false>
    {
      typedef SetMutable<SLICE> type;
    };
  }
}
