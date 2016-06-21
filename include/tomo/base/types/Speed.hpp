#pragma once

#include <gex/base.hpp>
#include <iostream>
#include <tbd/parse_utils.h>
#include "time.hpp"
#include <boost/operators.hpp>

namespace tomo
{
  namespace base
  {
    /**@brief Length type 
     * @detail Internally, length is stored in a float measured in mm
     */
    /*struct Length 
    {
      typedef gex::Scalar data_type;
      typedef long double litdata_type;

      Length() {}
      explicit Length(const data_type& _data) :
        data_(_data) {}
      explicit constexpr Length(litdata_type _data) : 
        data_(_data) {}

      static constexpr Length fromCentimeter(litdata_type _data)
      {
        return Length(_data * 10.0); 
      }

      static constexpr Length fromMeter(litdata_type _data)
      {
        return Length(_data * 1000.0); 
      }

      static constexpr Length fromInch(litdata_type _data)
      {
        return Length(_data * 25.4); 
      }
      
      operator data_type() const
      {
        return data_;
      }

      friend std::ostream& operator<< ( std::ostream& _os, const Length& _l )
      {
        _os << _l.data() << "mm";
        return _os;
      }

      friend std::istream& operator>> ( std::istream& _is, Length& _l )
      {
        Length::data_type _d;
        std::string _unit;
        tbd::parseTokenWithUnit(_is,_l.data(),_unit);
        if (_unit == "mm") _l.data(_d); else
        if (_unit == "cm") _l.fromCentimeter(_d); else
        if (_unit == "m") _l.fromMeter(_d); else
        if (_unit == "in") _l.fromInch(_d); 
        return _is;
      }

      TBD_PROPERTY_REF(data_type,data)
    };


    typedef gex::base::Range<Length> LengthRange;
*/    
    /**@brief Speed type 
     * @detail Internally, speed is stored in mm/sec
     */
    class Speed 
    {
    public:
      typedef gex::Scalar data_type;
      typedef long double litdata_type;

      Speed() {}
      Speed(const data_type& _data) :
        data_(_data) {}

      template<typename LENGTH>
      friend Speed operator / (const LENGTH& _length, const Duration& _duration)
      {
        return _length / _duration.count();
      }
 
      Speed toMMmin() const
      {
        return fromMMmin(this->data());
      }

      static Speed fromMMmin(const data_type& _data)
      {
        return Speed(_data * mmMinFactor());
      }
 
      static Speed toMMmin(const data_type& _data)
      {
        return Speed(_data / mmMinFactor());
      }

      operator data_type() const
      {
        return data_;
      }
      
      operator data_type() 
      {
        return data_;
      }

      TBD_PROPERTY_REF(data_type,data)

    private:
      static data_type mmMinFactor() { return 60.0; }

    };
    typedef gex::base::Range<Speed> SpeedRange;

  /*
    /// Temperature type. Stores as Kelvin internally
    struct Temperature
    {
      typedef gex::Scalar data_type;
      typedef long double litdata_type;

      Temperature() {}
      Temperature(const data_type& _data) :
        data_(_data) {}
      explicit constexpr Temperature(litdata_type _data) :
        data_(_data) {}

      static Temperature fromCelsius(litdata_type _data)
      {
      }
      
      static Temperature fromFahrenheit(litdata_type _data)
      {
      }
    };

    typedef gex::base::Range<Temperature> TemperatureRange;
  }*/

  }
  /*using base::Duration;
*/
  using base::Speed;
/*
  using base::Temperature;
  /// Millimeter
  inline constexpr Length operator"" _mm (Length::litdata_type _d)
  {
    return Length(_d);
  }
  
  /// Centimeter
  inline constexpr Length operator"" _cm (Length::litdata_type _d)
  {
    return Length::fromCentimeter(_d);
  }
  
  /// Meter
  inline constexpr Length operator"" _m (Length::litdata_type _d)
  {
    return Length::fromMeter(_d);
  }
  
  /// Inches
  inline constexpr Length operator"" _in (Length::litdata_type _d)
  {
    return Length::fromInch(_d);
  }
*/  
  inline constexpr Duration operator"" _ms (long double _d)
  {
    return Duration(_d / 1000.0);
  }

  inline constexpr Duration operator"" _sec (long double _d)
  {
    return Duration(_d);
  }
  
  inline constexpr Duration operator"" _min (long double _d)
  {
    return Duration(_d * 60.0);
  }
  
  inline constexpr Duration operator"" _h (long double _d)
  {
    return Duration(_d * 60.0 * 60.0);
  }
/*
  inline constexpr Speed operator"" _mm_s (long double _d)
  {
    return Speed(_d);
  }
  
  inline constexpr Speed operator"" _mm_min (long double _d)
  {
    return Speed().fromMMmin(_d);
  }
  
  inline constexpr Speed operator"" _inch_min (long double _d)
  {
    return Speed().fromInchMin(_d);
  }
  
  inline constexpr Speed operator"" _inch_sec (long double _d)
  {
    return Speed().fromInchSec(_d);
  }

  /// Kelvin
  inline constexpr Temperature operator"" _K (long double _t)
  {
    return Temperature(_t);
  }
  
  /// Celsius
  inline constexpr Temperature operator"" _C (long double _t)
  {
    return Temperature().fromCelsius(_d);
  }
  
  /// Fahrenheit
  inline constexpr Temperature operator"" _F (long double _t)
  {
    return Speed().fromFahrenheit(_t);
  }
*/
}
