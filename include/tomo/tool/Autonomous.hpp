#pragma once

#include "Interface.hpp"

namespace tomo
{
  namespace tool
  {
    template<typename ELEMENT, typename INTERFACE>
    class Autonomous : 
      public INTERFACE,
      public Interface
    {
    public:
      TOMO_BASE_TYPES
      typedef typename toolset_type::device_type device_type;
      typedef ELEMENT element_type;
      typedef base::Element const* element_ptr_type;
      typedef element_type const* ptr_type;
      
      Autonomous(mutable_parent_type _parent = nullptr) : 
        base_type(_parent) {}

      toolset_type const* toolset() const
      {
        if (!get()) return nullptr;
        return get()->toolset();
      }

      ptr_type get() const
      {
        if (this->element()->getTypeId() != element_type::typeId()) return nullptr;
        return static_cast<ptr_type>(this->element());
      }
      
      ptr_type get()
      {
        if (this->element()->getTypeId() != element_type::typeId()) return nullptr;
        return static_cast<ptr_type>(this->element());
      }
    };
  }
}

#define TOMO_TOOL_AUTONOMOUS(name,suffix)\
  TOMO_TOOL_TYPES\
  typedef typename base_type::element_type element_type;\
  typedef typename base_type::element_ptr_type element_ptr_type;\
  TOMO_TYPEID_DECL(element_type::typeId() + toolset_type::separator() + suffix)

