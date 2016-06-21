#pragma once

#include "SourceHandler.hpp"
#include "PortInterface.hpp"

namespace tomo
{
  namespace item
  {
    namespace detail
    {
      template<typename...ITEMS>
      struct MultiSource;

      template<typename...ITEMS>
      struct Source {};
    }

    class SourceInterface : public PortInterface
    {
      friend tool::Chain;
      virtual void link(item_interface_type const* _item) = 0;
      virtual void unlink(int _index = -1) = 0;

    public:
      static Id typeId() { return "Source"; }
      PortType portType() const { return SOURCE; }
    };
  }
  
  using item::SourceInterface;
}

namespace tbd
{
  template<typename...ITEMS>
  struct TypeInfo<tomo::item::detail::Source<ITEMS...>>
  {
    typedef tomo::Id token_type;
    token_type operator()()
    {
      return tomo::item::SourceInterface::typeId();
    }
  };
  
  template<typename...ITEMS>
  struct TypeInfo<tomo::item::detail::MultiSource<ITEMS...>>
  {
    typedef tomo::Id token_type;
    token_type operator()()
    {
      return tomo::item::SourceInterface::typeId();
    }
  };
}
