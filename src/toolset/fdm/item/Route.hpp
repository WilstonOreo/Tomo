#pragma once

#include "../routing/routing.hpp"
#include <tomo/item/Item.hpp>

namespace tomo
{
  namespace fdm
  {
    namespace item
    {
      class Route : public Item<Route>
      {
        TOMO_PARAMETER_LIST
        (
          (Scalar) fan
        )
      public:
        TOMO_ITEM("fdm",Route)

        Route(parent_type const* _parent) :
          Item<Route>(_parent) {}

        TBD_PROPERTY_REF(fdm::Route,data)
      };
    }
  }
}
