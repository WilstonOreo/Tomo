#pragma once

#include <tomo/base/Serialization.hpp>
#include "Interface.hpp"

namespace tomo
{
  namespace visual
  {
    template<typename DERIVED, typename ELEMENT>
    class Visualizer :
      public Interface,
      public Serializer<DERIVED>,
      public Registrar<DERIVED>
    {
    public:
      typedef ELEMENT element_type;

      Visualizer(Element const* _element) : 
        Interface(_element)
      {}

      inline element_type const* get() const
      {
        if (!element()) return nullptr;
        return static_cast<element_type const*>(element());
      }

      inline static ElementId typeId()
      {
        return element_type::typeId();
      }

      inline ElementId elementId() const
      {
        return typeId();
      }
    };
  }
  using visual::Visualizer;
}
