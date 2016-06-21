#pragma once

#include <tomo/base/Element.hpp>

namespace tomo
{
  namespace item
  {
    class Interface;

    class PortInterface : public virtual SerializationInterface
    {
    public:
      TOMO_BASE_TYPES
      enum PortType { SOURCE, SINK };
        
      typedef Interface item_interface_type;

      virtual ElementIdSet usedTypes(int _index = -1) const = 0;
      virtual ElementIdSet supportedTypes() const = 0;

      virtual bool empty() const = 0;
      virtual size_t number() const { return empty() ? 0 : 1; }
      virtual bool isMulti() const { return false; }

      virtual item_interface_type const* item(int _index = -1) const = 0;

      virtual PortType portType() const = 0;

      bool load(const ConfigPath& _path, const Config& _config)
      {
        return false;
      }
      void save(const ConfigPath& _path, Config& _config) const {}
    };
  }
}
