#pragma once

#include <QPainterPath>
#include "caption.hpp"

namespace tomo
{
  namespace widget
  {
    namespace paint
    {
      struct ToolNode
      {
        static inline float radius()
        {
          return 120.0;
        }

        static inline QRectF rect()
        {
          return QRectF(0.0,0.0,radius(),radius());
        }

        static inline QPainterPath shape()
        {
          QPainterPath path;
          path.addEllipse(rect());
          return path;
        }


        template<typename COLOR>
        static inline void paint(QPainter* _painter,
                                 const COLOR& _lightColor,
                                 const COLOR& _darkColor,
                                 const COLOR& _fontColor,
                                 const base::ElementId& _elementId,
                                 const std::string& _id = std::string(),
                                 bool _valid = true,
                                 bool _selected = false,
                                 float _progress = 100.0)
        {
          auto&& _shape = shape();
          auto&& _rect = shape().boundingRect();
          auto _radius = _rect.width();
          _radius /= _selected ? 1.5 : 1.75;

          QRadialGradient _gradient(_rect.width()/3.0, _rect.height()/3.0, _radius);
          _gradient.setColorAt(0,_lightColor);
          _gradient.setColorAt(1,_darkColor);

          bool _ready = _progress >= 100.0;

          auto _paintShape = [&_painter](const QBrush& _brush, const QPen& _pen, const QPainterPath& _shape)
          {
            _painter->setBrush(_brush);
            _painter->setPen(_pen);
            _painter->drawPath(_shape);
          };

          _painter->setBrush(_gradient);
          _painter->setPen(Qt::NoPen);

          if (!_ready || !_valid)
          {
            QRadialGradient _selectGradient(_rect.width()/1.25,_rect.width()/1.25,radius()*1.5);
            QColor _color(_lightColor);
            _color.setAlpha(128);
            _selectGradient.setColorAt(1.0,_color);
            _selectGradient.setColorAt(0.0,QColor(255,255,255,0));
            _painter->setBrush(_selectGradient);
            _painter->setPen(Qt::NoPen);
            _painter->drawEllipse(_rect);

            QRadialGradient _highlightGradient(_rect.width()/1.5,_rect.height()/1.5,_radius*0.33);
            _highlightGradient.setColorAt(0,QColor(255,255,255,0));
            _highlightGradient.setColorAt(0.95,QColor(255,255,255,0));
            _paintShape(_highlightGradient,QPen(QColor(0,0,0, 127.0)),_shape);
          }
          else
          {
            _paintShape(_gradient,QPen(QColor(0,0,0),1),_shape);
          }

          /// Draw Selected Highlight
          if (_selected)
          {
            QRadialGradient _selectGradient(_rect.width()*0.5,_rect.width()*0.5,_radius*0.8);
            _selectGradient.setColorAt(1,_lightColor);
            _selectGradient.setColorAt(0.85,QColor(255,255,255,0));
            _paintShape(_selectGradient,QPen(_darkColor,1),_shape);
          }

          auto _toolset = _elementId.toolset().str();
          if (_toolset != "generic")
          {
            auto font = _painter->font();
            font.setBold(true);
            font.setPointSize(radius()*0.25);
            _painter->setFont(font);
            auto _color = _fontColor;
            _color.setAlpha(64);
            _painter->setPen(_color);
            QTextOption textOption(Qt::AlignHCenter);
            textOption.setWrapMode(QTextOption::WordWrap);

            auto textRect = _rect;
            textRect.setTop(textRect.top() + _rect.height()/2 - font.pointSize());
            textRect.setBottom(textRect.bottom() - _rect.height()/2 + font.pointSize());
            _painter->drawText(textRect, QString().fromStdString(_toolset),textOption);
          }

          if (!_valid)
          {
            auto font = _painter->font();
            font.setBold(true);
            font.setPointSize(radius()*0.6);
            _painter->setFont(font);
            auto _color = _lightColor;
            _color.setAlpha(96);
            _painter->setPen(_color);
            QTextOption textOption = QTextOption(Qt::AlignHCenter);
            textOption.setWrapMode(QTextOption::WordWrap);

            auto textRect = _rect;
            textRect.setTop(textRect.top() + _rect.height()/1.8 - font.pointSize() );
            textRect.setBottom(textRect.bottom() - _rect.height()/2.0 + font.pointSize() * 2.0);
            _painter->drawText(textRect, QString("!"),textOption);
          }
          caption(_painter,_elementId.typeId().str(),_fontColor,_rect);
        }
      };
    }
  }
}
