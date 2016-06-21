#pragma once

#include <QPainterPath>
#include <QRectF>
#include <QPointF>

namespace tomo
{
  namespace widget
  {
    namespace paint
    {
      template<typename PAINTER, typename TEXT, typename COLOR>
      static void caption(
        PAINTER* _painter,
        TEXT const& _text,
        COLOR const& _fontColor,
        QPointF const& _point)
      {
        QPainterPath path;
        auto font = _painter->font();
        font.setBold(true);
        font.setPointSize(10.0);
        _painter->setFont(font);
        _painter->setBrush(_fontColor);
        _painter->setPen(Qt::NoPen);
        path.addText(0,0, font, QString().fromStdString(_text));
        path.translate(_point);
        _painter->drawPath(path);
      }

      template<typename PAINTER, typename TEXT, typename COLOR>
      static void caption(
        PAINTER* _painter,
        TEXT const& _text,
        COLOR const& _fontColor,
        QRectF const& _rect)
      {
        QPainterPath path;
        auto font = _painter->font();
        font.setBold(true);
        font.setPointSize(10.0);
        _painter->setFont(font);
        _painter->setBrush(_fontColor);
        _painter->setPen(Qt::NoPen);
        path.addText(0,0, font, QString().fromStdString(_text));
        path.translate(QPointF(_rect.width()/2.0,_rect.height()/2.0 + 2.0*font.pointSize())  -  path.boundingRect().center());
        _painter->drawPath(path);
      }
    }
  }
}
