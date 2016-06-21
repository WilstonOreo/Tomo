#pragma once

#include <gex/base.hpp>
#include <tbd/serialize.h>
#include "Parameter.hpp"
#include <tomo/visual/Color.hpp>
#include <tomo/visual/ColorRange.hpp>

namespace tbd
{
  TBD_BASE_TYPETOSTR(gex::Vec2,"vec2")
  TBD_BASE_TYPETOSTR(gex::Vec3,"vec3")
  TBD_BASE_TYPETOSTR(tomo::Color3,"color3")
  TBD_BASE_TYPETOSTR(tomo::Color4,"color4")
  TBD_BASE_TYPETOSTR(tomo::ColorRange3,"colorrange3")
  TBD_BASE_TYPETOSTR(tomo::ColorRange4,"colorrange4")
  TBD_BASE_TYPETOSTR(gex::Bounds2,"bounds2")
  TBD_BASE_TYPETOSTR(gex::Bounds3,"bounds3")
  TBD_BASE_TYPETOSTR(tomo::Bool,"bool")
  TBD_BASE_TYPETOSTR(tomo::String,"string")
  TBD_BASE_TYPETOSTR(tomo::Int,"int")
  TBD_BASE_TYPETOSTR(tomo::Float,"float")
  TBD_BASE_TYPETOSTR(tomo::QuotedString,"quotedstring")
}

namespace tomo
{
  namespace base
  {
    using tbd::TypeToStr;
  }
}


