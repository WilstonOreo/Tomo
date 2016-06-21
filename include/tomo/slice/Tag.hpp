#pragma once

#include <set>

namespace tomo
{
  namespace slice
  {
    typedef int Tag;
    typedef std::set<Tag> TagSet;

    static inline constexpr Tag invalidTag() { return -1; } 
  }
}
