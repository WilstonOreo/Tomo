#pragma once

#include <tbd/classregister.h>
#include "tomo/item/Interface.hpp"

namespace tomo
{
  namespace tool
  {
    using base::Element;
    using base::Id;
    using base::IdSet;
    using base::Ptr;

    class Interface;
    /// Registry for all registered tools
    typedef tbd::ClassRegistry<
      ElementId,
      Interface,Element::parent_type const*> Registry;

    /// Generic tool interface
    class Interface : public Element
    {
    public:
      typedef tool::Registry registry_type;
      typedef std::map<id_type,SourceInterface*> source_map_type;
      typedef std::map<id_type,SinkInterface*> sink_map_type;
      Interface(parent_type const* _parent) : Element(_parent) {}
      virtual ~Interface() {}

      /// A tool is valid if it can be processed
      virtual bool valid() const
      {
        return parent() != nullptr;
      }

      /// Processes tool
      virtual void process() = 0;

      virtual source_map_type sources() const = 0;
      virtual sink_map_type sinks() const = 0;

    private:
      friend Chain;
      virtual SourceInterface* getSource(const id_type& _id) = 0;
      virtual SourceInterface const* getSource(const id_type& _id) const = 0;
      virtual SinkInterface* getSink(const id_type& _id) = 0;
      virtual SinkInterface const* getSink(const id_type& _id) const = 0;
    };

#ifndef TOMO_PLUGIN
    /// A tool needs to inherit from Registrar to be registered
    template<typename T>
    using Registrar = tbd::ClassRegistrar<Registry,T>;
#endif
#ifdef TOMO_PLUGIN
    template<typename T>
    struct Registrar {};
#endif
  }
}

#define TOMO_TOOL(toolset,name)\
      TOMO_ELEMENT(toolset,name)\
      typedef tomo::tool::Registry registry_type;\
      typedef std::map<id_type,SourceInterface*> source_map_type;\
      typedef std::map<id_type,SinkInterface*> sink_map_type;

#ifndef TOMO_TEST
#define TOMO_TOOL_PLUGIN_INTERFACE(name)\
  extern "C" { void reg(){ tomo::tool::Registry::reg<name>(); }}
#endif
#ifdef TOMO_TEST
#define TOMO_TOOL_PLUGIN_INTERFACE(name)
#endif
