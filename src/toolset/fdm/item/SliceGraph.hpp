#pragma once

#include "../routing/slice.hpp"
#include <tomo/item/Item.hpp>

namespace tomo
{
  namespace fdm
  {
    namespace item
    {
      class SliceGraph : public Item<SliceGraph>
      {
        TOMO_PARAMETER_LIST
        (
          (Scalar) layer_height
        )
      public:
        TOMO_ITEM("fdm",SliceGraph)

        SliceGraph(parent_type const* _parent) :
          Item<SliceGraph>(_parent),
          layer_height_(0.0) {}

        Scalar& layer_height() 
        {
          return layer_height_;
        }

        void clear()
        {
          data_.clear();
        }

        slice::Graph& data()
        {
          return data_;
        }

        TBD_PROPERTY_REF_RO(slice::Graph,data)
      };
    }
  }
}
