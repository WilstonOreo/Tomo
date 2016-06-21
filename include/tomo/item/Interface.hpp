#pragma once

#include "tomo/base/Element.hpp"
#include "tomo/base/Storage.hpp"


namespace tomo
{
  namespace item
  {
    using base::Element;
    using base::Ptr;
    class Interface;

    class SinkInterface;
    class FileSourceInterface;

    /// Registry in which type Ids of all items are stored
    typedef tbd::ClassRegistry<
    ElementId,
    Interface,
    Element::parent_type const*> Registry;

    /// Item Interface
    class Interface : public Element
    {
    public:
      Interface(
        parent_type const* _parent) :
        Element(_parent) {}
      virtual ~Interface() {}

      typedef Registry registry_type;
      typedef void const* const_void_ptr_type;
      typedef void* mutable_void_ptr_type;

      bool hasChildren() const
      {
        return false;
      }

      virtual FileSourceInterface const* fileSource() const = 0; 
      virtual FileSourceInterface* fileSource() = 0; 

      virtual SinkInterface const* getSink() const = 0;
      virtual SinkInterface* getSink() = 0;
    };

#ifndef TOMO_PLUGIN
    /// Item Registrar. An item is needs to be inherited from Registrar to be registered in registry
    template<typename T>
    using Registrar = tbd::ClassRegistrar<Registry,T>;
#endif
#ifdef TOMO_PLUGIN
    template<typename T>
    struct Registrar {};
#endif
    
    typedef base::Storage<item::Registry> Storage;
  }
}

#define TOMO_ITEM(toolset,name)\
      TOMO_ELEMENT(toolset,name)\
      typedef tomo::item::Registry registry_type;

#include "SourceAndSink.hpp"
