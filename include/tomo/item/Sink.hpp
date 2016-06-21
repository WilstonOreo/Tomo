#pragma once

#include <tomo/item/Interface.hpp>
#include "detail/Sink.hpp"
#include "detail/AnySink.hpp"
#include "detail/SpecificSink.hpp"
#include "detail/VariantSink.hpp"
//#include "detail/MultiSink.hpp"

#include "detail/ItemSink.hpp"
#include "FileSink.hpp"

namespace tomo
{
  namespace item
  {
    using detail::Sink;
  //  using detail::MultiSink;
  }
  using item::Sink;
  using item::AnySink;
  using item::VariantSink;
//  using item::SingleSink;
//  using item::MultiSink;
}
