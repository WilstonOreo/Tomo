#pragma once

#include <tomo/visual/Color.hpp>
#include <QColor>

namespace tomo
{
  namespace qt
  {
    static inline QColor color(const Color3& _color)
    {
      return QColor(_color.r()*255,_color.g()*255,_color.b()*255);
    }

    static inline QColor color(const Color4& _color)
    {
      return QColor(_color.r()*255,_color.g()*255,_color.b()*255,_color.a());
    }

    static inline Color3 color(const QColor& _color)
    {
      return Color3(_color.red()/255.0,_color.green()/255.0,_color.blue()/255.0);
    }
  }
}
