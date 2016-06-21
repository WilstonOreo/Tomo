#pragma once

#include <tomo/base/Element.hpp>
#include <tomo/base/Signal.hpp>

namespace tomo
{
  namespace visual
  {

    class Interface;
    /// Registry for all registered tools
    typedef tbd::ClassRegistry<ElementId,Interface,base::Element const*> Registry;


    inline static bool has_visualizer(base::ElementId const& _id)
    {
      return Registry::exists(_id);
    }

    inline static bool has_visualizer(base::Element const* _element)
    {
      return has_visualizer(_element->elementId());
    }

    class Interface :
      public base::SignalInterface,
      public base::Element
    {
    public:
      TOMO_BASE_TYPES
      typedef visual::Registry Registry;
      typedef gex::Bounds3 bounds_type;

      Interface(base::Element const* _element) :
        base::Element(nullptr),
        element_(nullptr)
      {
        element(_element);
      }

      virtual ~Interface() {}

      inline base::Element const* element() const
      {
        return element_;
      }

      inline void element(base::Element const* _element)
      {
        if (element_)
        {
          if (element_->elementId() != elementId())
          {
            element_ = nullptr;
            return;
          }
        }
        element_ = _element;
      }

      /// The display method
      virtual void process() = 0;

      /// Method used for preprocessing data for faster display
      inline virtual void preprocess() {};

      /// Method to initialize all necessary geometry needed
      /// Usually called after toolchain has been reprocessed
      inline virtual void initialize() { preprocess(); }

      virtual bounds_type bounds() const = 0;

    private:
      base::Element const* element_;
    };

    typedef std::set<Interface*> VisualizerList;


    #ifndef TOMO_PLUGIN
    /// A visualizer needs to inherit from Registrar to be registered
    template<typename T>
    using Registrar = tbd::ClassRegistrar<Registry,T>;
  #endif
  #ifdef TOMO_PLUGIN
    template<typename T>
    struct Registrar {};
  #endif
  }
}

#define TOMO_VISUALIZER(element)\
  TOMO_BASE_TYPES\
  typedef tomo::visual::Interface interface_type;\
  typedef element element_type;\
  typedef interface_type::bounds_type bounds_type;

#ifndef TOMO_TEST
#define TOMO_VISUALIZER_PLUGIN_INTERFACE(name)\
  extern "C" { void reg(){ tomo::visual::Registry::reg<name>(); }}
#endif
#ifdef TOMO_TEST
#define TOMO_VISUALIZER_PLUGIN_INTERFACE(name)
#endif
