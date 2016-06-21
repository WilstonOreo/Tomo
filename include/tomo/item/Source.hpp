#pragma once

#include <tomo/item/Interface.hpp>
#include "detail/Source.hpp"
#include "detail/AnySource.hpp"
#include "detail/SpecificSource.hpp"
#include "detail/VariantSource.hpp"
//#include "detail/MultiSource.hpp"
#include "FileSource.hpp"


namespace tomo
{
  namespace item
  {
    using detail::Source;
//    using detail::MultiSource;
  }

  using item::AnySource;
  using item::Source;
  using item::VariantSource;
  //using item::MultiSource;
}
