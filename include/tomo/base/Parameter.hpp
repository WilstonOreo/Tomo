#pragma once

#include <iostream>
#include <string>
#include <tbd/parse_utils.h>
#include <tbd/property.h>
#include <gex/base.hpp>

namespace tomo
{
  namespace base
  {
    template<typename T>
    struct Parameter
    {
      typedef T type;

      Parameter() {}
      Parameter( const type& _data ) : data_(_data) {}
      Parameter( type&& _data ) : data_(_data) {}

      operator type() const
      {
        return data_;
      }

      friend std::ostream& operator << ( std::ostream &out, const Parameter& _p )
      {
        out << _p.data();
        return out;
      }
      
      friend std::istream& operator >> ( std::istream &in, Parameter& _p )
      {
        in >> _p.data();
        return in;
      }

      TBD_PROPERTY_REF(type,data)
    };

    template<>
    struct Parameter<bool>
    {
      typedef bool type;

      Parameter() {}
      Parameter( const type& _data ) : data_(_data) {}
      Parameter( type&& _data ) : data_(_data) {}

      operator type() const
      {
        return data_;
      }

      friend std::ostream & operator << ( std::ostream &out, const Parameter& _p )
      {
        out << std::boolalpha << _p.data();
        return out;
      }
      
      friend std::istream & operator >> ( std::istream &in, Parameter& _p )
      {
        in >> std::boolalpha >> _p.data();
        return in;
      }

      TBD_PROPERTY_REF(type,data)
    };

/*
    struct RotationZ : Parameter<gex::Scalar>
    {

    };

    struct RotationXYZ : Parameter<gex::Vec3>
    {
    };

    struct UniformScale : Parameter<gex::Scalar>
    {
    };

    struct NonUniformScale : Parameter<gex::Vec3>
    {
    };

    struct TranslationXY : Parameter<gex::Vec2>
    {
    };

    struct TranslationXYZ : Parameter<gex::Vec3>
    {
    };
*/
    struct QuotedString : Parameter<std::string>
    {
      typedef Parameter<std::string> base_type;
      typedef typename base_type::type type;

      QuotedString() {}
      template<typename DATA>
      QuotedString( const DATA& _data ) : base_type(_data) {}
      template<typename DATA>
      QuotedString( DATA&& _data ) : base_type(_data) {}

      operator type() const
      {
        return data();
      }

      friend std::ostream & operator << ( std::ostream &out, const QuotedString& _s )
      {
        out << '"' << _s.data() << '"';
        return out;
      }
      friend std::istream & operator >> ( std::istream &in, QuotedString& _s )
      {
        in >> _s.data();
        _s.data() = tbd::removeQuotes(_s.data());
        return in;
      }
    };
  }

  typedef base::Parameter<int> Int;
  typedef base::Parameter<gex::Scalar> Float;
  typedef base::Parameter<bool> Bool;
  typedef base::Parameter<std::string> String;
  using base::QuotedString;
}
