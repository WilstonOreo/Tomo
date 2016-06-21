#pragma once

#include <tbd/property.h>
#include "Tag.hpp"

namespace tomo
{
  namespace slice
  {
    /// Generic slice type
    template<typename POLYGON>
    struct Generic
    {
      typedef Tag tag_type;
      typedef POLYGON polygon_type;

      Generic(const tag_type& _tag = invalidTag()) :
        tag_(_tag) {}

      Generic(const polygon_type& _polygon,
              const tag_type& _tag = invalidTag()) :
        tag_(_tag),
        polygon_(_polygon) {}

      TBD_PROPERTY_REF(tag_type,tag)
      TBD_PROPERTY_REF_RO(polygon_type,polygon)
    protected:
      polygon_type& polygon() 
      {
        return polygon_;
      }
    };
  }
}
