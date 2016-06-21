#pragma once

#include <tomo/base/gex.hpp>
#include "Color.hpp"

namespace tomo
{
  namespace visual
  {
    template<typename COLOR>
    struct ColorRange
    {
      typedef COLOR color_type;
      typedef Range range_type;

      ColorRange() {}
      ColorRange(
          const range_type& _range,
          const color_type& _minColor,
          const color_type& _maxColor) :
        range_(_range),
        minColor_(_minColor),
        maxColor_(_maxColor) {}
      
      template<typename SCALAR>
      color_type interpolate(const SCALAR& _pos)
      {
        if (range_.length() == 0.0) return minColor_;
        SCALAR _factor = (_pos - range_.min()) / range_.length();
        _factor = std::min(SCALAR(1.0),std::max(_factor,SCALAR(0.0)));
        return (1 - _factor) * minColor_ + _factor * maxColor_;
      }

      friend bool operator==(const ColorRange& _lhs, const ColorRange& _rhs)
      {
        return (_lhs.range_ == _rhs.range_);
      }

      TBD_PROPERTY_REF(range_type,range)
      TBD_PROPERTY_REF(color_type,minColor)
      TBD_PROPERTY_REF(color_type,maxColor)
    };

    typedef ColorRange<Color3> ColorRange3;
    typedef ColorRange<Color4> ColorRange4;
  }

  using visual::ColorRange3;
  using visual::ColorRange4;
}

namespace tbd
{ 
  template<typename COLOR>
  struct Serialize<tomo::visual::ColorRange<COLOR>>
  {
    template<typename T, typename CONFIG>
    static bool load(T& _t, const tbd::ConfigPath& _path, CONFIG const& _config)
    { 
      return 
       Serialize<tomo::Range>::load(_t.range(),_path / "range",_config) |
       Serialize<COLOR>::load(_t.minColor(),_path / "min_color",_config) |
       Serialize<COLOR>::load(_t.maxColor(),_path / "max_color",_config);
    }

    template<typename T, typename CONFIG>
    static void save(const T& _t, const tbd::ConfigPath& _path, CONFIG& _config)
    {
      Serialize<tomo::Range>::save(_t.range(),_path / "range",_config);
      Serialize<COLOR>::save(_t.minColor(),_path / "min_color",_config);
      Serialize<COLOR>::save(_t.maxColor(),_path / "max_color",_config);
    }
  };  
}

