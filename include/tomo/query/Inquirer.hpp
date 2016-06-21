#pragma once

#include "Interface.hpp"

namespace tomo
{
  namespace query
  {
    template<typename DERIVED, typename ELEMENT>
    class Inquirer : 
      public Interface,
      public Serializer<DERIVED>,
      public Registrar<DERIVED>
    {
    public:
      typedef ELEMENT element_type;

      Inquirer(Element const* _element) : 
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
  using query::Inquirer;
}

