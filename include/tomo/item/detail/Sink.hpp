#pragma once

#include "SinkHandler.hpp"
#include "Source.hpp"

namespace tomo
{
  namespace item
  {
    namespace detail
    {
      template<typename...ITEMS>
      struct MultiSink;

      template<typename...ITEMS>
      struct Sink {};
    }

    class SinkInterface : public PortInterface
    {
      friend tool::Chain;
      virtual void link(SourceInterface const* _item) = 0;
      virtual void unlink(int _index = -1) = 0;
    public:
      PortType portType() const
      {
        return SINK;
      }
      static Id typeId()
      {
        return "Sink";
      }
    private:

    };
  }
  using item::SinkInterface;
}

namespace tbd
{
  template<typename...ITEMS>
  struct TypeInfo<tomo::item::detail::Sink<ITEMS...>>
  {
    typedef tomo::Id token_type;
    token_type operator()()
    {
      return tomo::item::SinkInterface::typeId();
    }
  };

  template<typename...ITEMS>
  struct TypeInfo<tomo::item::detail::MultiSink<ITEMS...>>
  {
    typedef tomo::Id token_type;
    token_type operator()()
    {
      return tomo::item::SinkInterface::typeId();
    }
  };
}


