#pragma once

#include <iostream>
#include <sstream>
#include <tbd/property.h>
#include <tomo/base/Id.hpp>

namespace tomo
{
  namespace routing
  { 
    namespace gcode
    {
      using base::Token;

      /// Class for storing a GCode field of the form LETTERVALUE (e.g. G100)
      struct Field 
      {
        typedef char letter_type;
        typedef double value_type;

        Field(letter_type _letter = 0, value_type _value = 0) :
          letter_(_letter), value_(_value)
        {}
        
        /// Construct a field by parsing it from a string
        Field(const Token& _t)
        {
          std::istringstream is(_t);
          is >> (*this);
        }
 
        /// Clear field (sets letter and value to zero)
        inline void clear()
        {
          letter_ = 0;
          value_ = 0;  
        }

        /// A field is empty if letter == 0 
        inline bool empty() const
        {
          return letter_ == 0; 
        }

        /// Read from stringstream and assign values
        friend std::istream& operator>>(std::istream& _is, Field& _cf)
        {
          /// Peek next letter to see if its a letter
          do 
          {
            _is >> _cf.letter();
            if (!_is.good()) return _is;
          } while (isspace(_is.peek()));

          if (std::isalpha(_cf.letter())) 
          {
            _is >> _cf.value();
          } else
          {
            _cf.clear();
          }

          return _is;
        }

        /// Generate stream output
        friend std::ostream& operator<<( std::ostream& _os, const Field& _cf)
        {
          _os << _cf.letter() << _cf.value();
          return _os;
        }

        /// Sort by letter
        friend bool operator<( const Field& _lhs, const Field& _rhs)
        {
          return _lhs.letter() < _rhs.letter();
        }

        /// Check if fields are equal
        friend bool operator==( const Field& _lhs, const Field& _rhs)
        {
          return (_lhs.letter() == _rhs.letter()) &&
                 (_lhs.value()  == _rhs.value());
        }

        friend bool operator== (const Field& _lhs, const Token& _token)
        {
          std::ostringstream _ss;
          _ss << _lhs;
          return _ss.str() == _token;
        }
        
        friend bool operator== (const Token& _token, const Field& _lhs)
        {
          return _token == _lhs;
        }

        friend bool operator!=( const Field& _lhs, const Field& _rhs)
        {
          return !(_lhs == _rhs);
        }

        TBD_PROPERTY_REF(letter_type,letter)
        TBD_PROPERTY_REF(value_type,value)
      };
    }
  }
}


