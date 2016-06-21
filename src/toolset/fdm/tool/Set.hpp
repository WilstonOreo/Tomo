#pragma once
#include <tomo/tool/Set.hpp>
#include <tomo/tool/Tool.hpp>
#include <tomo/item/Item.hpp>
#include "Device.hpp"
#include "GCodes.hpp"

namespace tomo
{
  namespace fdm
  { 
    class Toolset :
      public tomo::tool::Set<Device>,
      public SubobjectSerializer<Toolset,Device,GCodes>
    {
      TBD_PARAMETER_LIST
      (
        (Scalar) layer_height,
        (Scalar) extrusion_multiplier
      )

    public:
      typedef tomo::tool::Set<Device> base_type;
      typedef tbd::SubobjectSerializer<Toolset,Device,GCodes> serializer_type;

      Toolset(mutable_parent_type _parent) : 
        base_type(_parent),
        serializer_type(*this,device(),gcodes_),
        layer_height_(0.3),
        extrusion_multiplier_(1.0) 
      {}

      TOMO_TYPEID_DECL("fdm")
      TBD_PROPERTY_REF_RO(GCodes,gcodes)
    };
    
    template<typename DATA>
    using Item = tomo::item::Item<Toolset,DATA>;
    
    typedef tomo::tool::Tool<Toolset> Tool;
  }
}
