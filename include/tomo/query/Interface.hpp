#pragma once

#include "tomo/base/Element.hpp"
#include "tomo/base/Signal.hpp"

namespace tomo
{
  namespace query
  {
    class Interface;
    /// Registry for all registered tools
    typedef tbd::ClassRegistry<ElementId,Interface,base::Element const*> Registry;

    inline static bool has_inquirer(base::ElementId const& _id)
    {
      return Registry::exists(_id);
    }

    inline static bool has_inquirer(base::Element const* _element) 
    {
      return has_inquirer(_element->elementId());
    }

    class Interface : 
      public base::SignalInterface,
      public base::Element
    {
    public:
      typedef tbd::Config data_type;
      typedef base::Element const* element_ptr_type;
      
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



      inline virtual void process() = 0; 
      inline virtual void preprocess() {};
          
      TBD_PROPERTY_REF(data_type,data)
    private:
      base::Element const* element_;
    };
    
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

#define TOMO_INQUIRER(element)\
  TOMO_BASE_TYPES\
  typedef tomo::query::Interface interface_type;\
  typedef element element_type;\

#ifndef TOMO_TEST
#define TOMO_INQUIRER_PLUGIN_INTERFACE(name)\
  extern "C" { void reg(){ tomo::query::Registry::reg<name>(); }}
#endif
#ifdef TOMO_TEST
#define TOMO_INQUIRER_PLUGIN_INTERFACE(name)
#endif
