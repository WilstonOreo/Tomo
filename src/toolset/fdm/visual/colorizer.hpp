#pragma once

#include <tomo/visual/ColorRange.hpp>
#include "../routing/routing.hpp"

namespace tomo
{
  namespace fdm
  {
    namespace visual
    {
      template<typename RANGE, typename SCALAR, typename COLOR>
      COLOR interpolate(const RANGE& _range, const SCALAR& _pos, const COLOR& _a, const COLOR& _b)
      {
        SCALAR _factor = (_pos - _range.min()) / _range.length();
        return (1 - _factor) * _a + _factor * _b;
      }
      
      typedef gex::Color4<uint8_t> Color4ub; 

      Color4ub toColor4ub(const Color4& _color)
      {
        using gex::color::clamp;
        return Color4ub(
            clamp(int(255*_color.r()),0,255),
            clamp(int(255*_color.g()),0,255),
            clamp(int(255*_color.b()),0,255),
            clamp(int(255*_color.a()),0,255));
      }

      namespace colorizer
      {
        struct ExtrusionWidth : ColorRange3
        {
          ExtrusionWidth(const ColorRange3& _range) :
            ColorRange3(_range) {}

          template<typename T>
          Color4ub operator()(const T& _t)
          {
            return toColor4ub(Color4(ColorRange3::interpolate(_t.extrusionWidth())));
          }
        };

        struct Tag
        {
          Tag(int _tag = 0) : tag_(_tag) {}

          template<typename T>
          Color4ub operator()(const T& _t)
          {
            if (tag_ < 0 || tag_ >= colors().size()) return Color4ub();
            return toColor4ub(colors()[tag_]);
          }

          TBD_PROPERTY(int,tag)

        private:
          static const std::vector<Color4> colors()
          {
            static std::vector<Color4> colors_;
            if (colors_.empty())
            {
              colors_ =
              {
                Color4(1,1,1,1),
                Color4(1,1,0,1),
                Color4(1,0,1,1),
                Color4(1,0,0,1),
                Color4(0,1,1,1),
                Color4(0,1,0,1),
                Color4(0,0,1,1),
                Color4(0,0,0,1)
              };
            }

            return colors_;
          }
        };

        struct Speed : ColorRange3
        {
          Speed(const ColorRange3& _colorRange) : 
            ColorRange3(_colorRange) {}

          template<typename T>
          Color4ub operator()(const T& _t)
          {
            return toColor4ub(Color4(ColorRange3::interpolate(Scalar(_t.speed()))));
          }
        };

        struct Fan : ColorRange3
        {
          Fan(const ColorRange3& _colorRange) :
            ColorRange3(_colorRange) {} 

          template<typename T>
          Color4ub operator()(const T& _t)
          {
            return _t.fan() ? 
              toColor4ub(Color4(this->maxColor())) : 
              toColor4ub(Color4(this->minColor()));
          }
        };

        struct ExtruderTemperature : ColorRange3
        {
          ExtruderTemperature(const ColorRange3& _colorRange) :
            ColorRange3(_colorRange) {}

          template<typename T>
          Color4ub operator()(const T& _t)
          {
            return toColor4ub(Color4(ColorRange3::interpolate(Scalar(_t.extruder_temperature())))); 
          }
        };
      }
    }
  }
}
