#pragma once

#include <QPainterPath>
#include "caption.hpp"

namespace tomo
{
  namespace widget
  {
    namespace paint
    {
      struct ItemNode
      {
        static inline float radius()
        {
          return 120.0;
        }
          
        static inline float shift()
        {
          return 20.0;
        }

        static inline QRectF rect()
        {
          return QRectF(0.0,0.0,radius()+shift(),radius()*0.5);
        }

        static inline QPainterPath shape()
        {
          QPainterPath path;
          QPolygonF polygon;
          polygon.append(QPointF(shift(),0));
          polygon.append(QPointF(radius()+shift(),0));
          polygon.append(QPointF(radius(),radius()*0.5));
          polygon.append(QPointF(0,radius()*0.5));
          polygon.append(QPointF(shift(),0));
          path.addPolygon(polygon);
          return path;
        }


        template<typename COLOR>
        static inline void paint(QPainter* _painter,
                                 const COLOR& _lightColor,
                                 const COLOR& _darkColor,
                                 const COLOR& _fontColor,
                                 const base::ElementId& _elementId,
                                 const std::string& _id = std::string(),
                                 bool _selected = false)
        {
          auto&& _shape = shape();
          auto&& _rect = rect();
          auto _radius = _rect.width();
          _radius /= _selected ? 1.5 : 1.75;
          QLinearGradient _gradient(QPointF(0.0,_rect.top()),QPointF(0.0,_rect.bottom()));
          _gradient.setColorAt(0,_lightColor);
          _gradient.setColorAt(1,_darkColor);

          auto _paintShape = [&_painter](const QBrush& _brush, const QPen& _pen, const QPainterPath& _shape)
          {
            _painter->setBrush(_brush);
            _painter->setPen(_pen);
            _painter->drawPath(_shape);
          };

          _painter->setBrush(_gradient);
          _painter->setPen(Qt::NoPen);

          _paintShape(_gradient,QPen(QColor(0,0,0),1),_shape);

          /// Draw Selected Highlight
          if (_selected)
          {
            QRadialGradient _selectGradient(_rect.width()*0.5,_rect.width()*0.5,_radius*0.8);
            _selectGradient.setColorAt(1,_lightColor);
            _selectGradient.setColorAt(0.85,QColor(255,255,255,0));
            _paintShape(_selectGradient,QPen(_darkColor,1),_shape);
          }

          caption(_painter,_id,_fontColor,QPointF(shift() + 5,15));
          caption(_painter,_elementId.str(),_fontColor,QPointF(shift(),30));
        }
/*
        template<typename T, typename ID, typename SINKPORTS>
        static inline void ports(T* _t, const ID& _id, SINKPORTS& _ports)
        {
          auto&& _sink = _t->item()->getSink();
          _ports.insert(new widget::SinkPort(_t,_id,_sink);
          auto _port = *sinkPorts().begin();
          auto&& _rect = shape().boundingRect();
          //float _r = Port::radius() * 1.5;
          _port->setPos(_rect.width()-shift() - 10,radius()*0.5 - 10);
        }
*/
      };
    }
  }
}
