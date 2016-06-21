#pragma once

#include <exception>
#include <sstream>
#include <tbd/property.h>
#include "Element.hpp"

namespace tomo
{
  namespace base
  {
    namespace exception
    {
      /**@brief A generic parser exceptions
        *@detail Needs a line number as parameter
       **/
      class Parser : 
        public std::exception
      {
      public:
        Parser() {}

        inline virtual const char* what() const noexcept
        {
          std::stringstream ss;
          TOMO_ERR_(ss,"Parser Error :");
          return ss.str().c_str();   
        }
      };
      
      /**@brief An exception raised if TypeId was not found in Registry
        *@detail Needs typeId as parameter
       **/
      class InvalidTypeId : public Parser
      {
      public:
        InvalidTypeId(const Id& _typeId) :
          typeId_(_typeId) {}

        inline virtual const char* what() const noexcept
        {
          std::stringstream ss;
          ss << Parser::what() << " " << "Invalid type id: " << typeId_;
          return ss.str().c_str();
        }

        TBD_PROPERTY_REF_RO(Id,typeId)
      };

      /**@brief An exception raised if element is malformed
        *@detail Needs element as parameter
       **/
      class MalformedElement : public Parser
      {
      public:
        MalformedElement(const Element::token_type& _elementStr) :
        elementStr_(_elementStr) {}

        inline virtual const char* what() const noexcept
        {
          std::stringstream ss;
          ss << Parser::what() << " ";
          ss << "Malformed element: " << elementStr();
          return ss.str().c_str();
        }

        TBD_PROPERTY_REF_RO(Element::token_type,elementStr)
      };
    }
  }
}

