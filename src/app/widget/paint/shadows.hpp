#pragma once
#include <QGraphicsDropShadowEffect>

namespace tomo
{
  namespace widget
  {
    namespace paint
    {
      static inline QGraphicsDropShadowEffect* shadows(float _alpha = 128.0, float _radius = 20.0)
      {
        QGraphicsDropShadowEffect* _shadows = new QGraphicsDropShadowEffect();
        _shadows->setColor(QColor("000000"));
        _shadows->color().setAlpha(_alpha);
        _shadows->setBlurRadius(_radius);
        return _shadows;
      }
    }
  }
}

