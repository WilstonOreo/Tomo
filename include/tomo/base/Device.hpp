#pragma once

#include "Element.hpp"

namespace tomo
{
  namespace base
  {
    class NullDevice : Element 
    {
    public:
      TOMO_BASE_TYPES

      NullDevice(mutable_parent_type _parent) : 
        Element(_parent) {}

      TOMO_TYPEID_DECL("devnull")
    };
  }
}
