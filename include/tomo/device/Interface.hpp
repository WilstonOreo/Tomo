#pragma once

#include <tomo/base/Element.hpp>
#include <tomo/base/Storage.hpp>
#include <tomo/item/Source.hpp>

namespace tomo
{
  namespace device
  {
    using base::Ptr;
    using base::Element;
    using base::Id;
    using base::IdSet;

    class Interface;
    /// Registry for all registered tools
    typedef tbd::ClassRegistry<ElementId,Interface,Element::parent_type const*> Registry;

    /// Generic device interface
    class Interface : public Element
    {
    public:
      typedef device::Registry registry_type;
      typedef std::map<id_type,SourceInterface*> source_map_type;

      Interface(parent_type const* _parent) : Element(_parent) {}
      virtual ~Interface() {}

      bool hasChildren() const
      {
        return false;
      }

      virtual bool valid() const
      {
        return true;
      }

      virtual void process() = 0;
      virtual void saveToFile(const std::string& _filename) {}

      virtual source_map_type sources() const = 0;

    private:
      friend tool::Chain;
      virtual SourceInterface* getSource(const id_type& _id) = 0;
      virtual SourceInterface const* getSource(const id_type& _id) const = 0;
    };

#ifndef TOMO_PLUGIN
    /// A device needs to inherit from Registrar to be registered
    template<typename T>
    using Registrar = tbd::ClassRegistrar<Registry,T>;
#endif
#ifdef TOMO_PLUGIN
    template<typename T>
    struct Registrar {};
#endif
    
    typedef base::Storage<device::Registry> Storage;
  }
}

#define TOMO_DEVICE(toolset,name)\
      TOMO_ELEMENT(toolset,name)\
      typedef tomo::device::Registry registry_type;\
      typedef std::map<id_type,SourceInterface*> source_map_type;


#ifndef TOMO_TEST
#define TOMO_DEVICE_PLUGIN_INTERFACE(name)\
  extern "C" { void reg(){ tomo::device::Registry::reg<name>(); }}
#endif
#ifdef TOMO_TEST
#define TOMO_DEVICE_PLUGIN_INTERFACE(name)
#endif

