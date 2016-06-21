
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include <gex/qt.hpp>

#include "widget/tool/Edge.h"
#include "widget/tool/Node.h"
#include "widget/tool/Port.h"
#include "widget/ToolchainEditor.h"

namespace tomo
{
  namespace widget
  {
    namespace tool
    {
      Node::Node(
        ToolchainEditor* _parent, tool::Interface* _tool) :
        base_type(_parent),
        prev_(nullptr),
        next_(nullptr),
        text_(_text),
        editor_(_editor)
      {
        setFlag(ItemIsMovable);
        setFlag(ItemSendsGeometryChanges);
        setCacheMode(DeviceCoordinateCache);
        setZValue(2.0);
      }

      QRectF Node::boundingRect() const
      {
        qreal adjust = 2;
        QRectF _boundingRect = rect();
        _boundingRect.adjust(-adjust,-adjust,adjust,adjust);
        return _boundingRect;
      }

      Node::sourceport_list& Node::sourcePorts()
      {
        return sourcePorts_;
      }

      Node::sinkport_list& Node::sinkPorts()
      {
        return sinkPorts_;
      }

      QRectF Node::rect()
      {
        return QRectF(-radius(),-radius(),radius(),radius());
      }

      float Node::radius()
      {
        return 120.0;
      }

      QPainterPath Node::shape() const
      {
        QPainterPath path;
        path.addEllipse(rect());
        return path;
      }

      void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
      {
        auto&& _shape = shape();
        auto _radius = rect().width();
        _radius /= this->isSelected() ? 1.5 : 1.75;

        auto _rect = rect();
        auto _progress = 
          this->isValid() ? clamp(progress(),0,100) : 0.0;

        auto _lightColor = light();
        auto _darkColor = dark();

        QRadialGradient _gradient(-_rect.width()/1.5, -_rect.height()/1.5, _radius);

        auto _paintShape = [&_painter](const QBrush& _brush, const QPen& _pen, const QPainterPath& _shape)
        {
          _painter->setBrush(_brush);
          _painter->setPen(_pen);
          _painter->drawPath(_shape);
        };

        if (!ready())
        {
          QRadialGradient _selectGradient(_rect.width()/1.25,_rect.width()/1.25,radius()*1.5);
          QColor _color(light);
          _color.setAlpha(128);
          _selectGradient.setColorAt(1.0,_color);
          _selectGradient.setColorAt(0.0,QColor(255,255,255,0));
          _painter->setBrush(_selectGradient);
          _painter->setPen(Qt.NoPen);
          _painter->drawEllipse(_rect);

          QPainterPath _path(_shape);
          QPainterPath _rectPath;
          auto _boundingRect = _shape.boundingRect();
          _boundingRect.setBottom(_rect.bottom() - _rect.height() * _progress / 100.0);
          _rectPath.addRect(_boundingRect);
          if (isOpened())
          {
            QPainterPath _ellipsePath;
            _ellipsePath.addEllipse(_rect);
            _rectPath &= _ellipsePath;
          }
          _path -= _rectPath;

          _paintShape(_gradient,Qt.NoPen,_path);

          QRadialGradient _highlightGradient(_rect.width()/1.5,_rect.height()/1.5,_radius*0.33);
          _highlightGradient.setColorAt(0,QColor(255,255,255,255 - pow(progress/100.0,3.0) * 255));
          _highlightGradient.setColorAt(0.95,QColor(255,255,255,0));
          _paintShape(_highlightGradient,QPen(QColor(0,0,0,(1 + _progress /100.0 )* 127.0)),_shape);
        } else
        {
          _paintShape(_gradient,QPen(QtGui.QColor(0,0,0),1));
        }

        /// Draw Selected Highlight
        if (isSelected())
        {
          QRadialGradient _selectGradient(_rect.width()*0.5,_rect.width()*0.5,_radius*0.5);
          _selectGradient.setColorAt(1,_light);
          _selectGradient.setColorAt(0.85,QColor(255,255,255,0));
          _paintShape(_selectGradient,QPen(_dark,1),_shape);
        }
        
        if (isOpened())
        {
          /// Draw Dialog gradient
          auto _dialogRect = translatedDialogRect();
          QPainterPath _ellipsePath;
          _ellipsePath.addEllipse(_rect);
          QPainterPath _rectPath;
          _rectPath.addRoundedRect(_dialogRect,ToolPort.radius(),ToolPort.radius());
          _rectPath -= _ellipsePath;

          QLinearGradient _linearGradient(QPointF(0,_dialogRect.top()),QPoint(0,_dialogRect.bottom()));
          /// !!!TODO......... ToolNode py 154

          _paintShape(_gradient,Qt.NoPen,_rectPath);
        }
      }

      QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
      {
        switch (change)
        {
        case ItemPositionHasChanged:
        {
          if ((opened_ && resizing_) || portHovered_)
          {
            setPos(lastPos_);
            adjust();
            return QGraphicsItem::itemChange(change, value);
          }

          


          if (prev_) prev_->adjust();
          if (next_) next_->adjust();
          editor_->itemMoved();
          break;
        }
        default:
          break;
        };

        return QGraphicsItem::itemChange(change, value);
      }

      void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
      {
        update();
        QGraphicsItem::mousePressEvent(event);
      }

      void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
      {
        update();
        QGraphicsItem::mouseReleaseEvent(event);
      }

      void Node::update()
      {
        gex::Range _srcDegRange(0,180);
        gex::Scalar _srcSectionAngle = _srcDegRange.length() / sourcePorts_.size();

        gex::Scalar _srcDelta = 90 + _srcDegRange.min() + _srcSectionAngle * 0.5;
        gex::Point2 _dir(1.0,0.0);

        if (prev_ && next_)
        {
          _dir = (prev_->dir() + next_->dir()).normalized();
        }
        else if (next_)
        {
          _dir = next_->dir();
        }
        else if (prev_)
        {
          _dir = prev_->dir();
        }

        QPointF _center = pos() - rect().bottomRight() + QPoint(Port::rect().width()*0.5,Port::rect().height()*0.5);

        for (auto _port : sourcePorts_)
        {
          gex::Point2 _t = Eigen::Rotation2D<gex::Scalar>(gex::deg2rad(_srcDelta)) * _dir * radius()*0.4 + gex::qt::adapt(rect().center());
          _port->setPos(_center + gex::qt::adapt(_t));
          _srcDelta += _srcSectionAngle;
        }

        _srcSectionAngle = _srcDegRange.length() / sinkPorts_.size();
        _srcDelta = 90 + _srcDegRange.max() + _srcSectionAngle * 0.5;

        for (auto _port : sinkPorts_)
        {
          gex::Point2 _t = Eigen::Rotation2D<gex::Scalar>(gex::deg2rad(_srcDelta)) * _dir * radius()*0.4 + gex::qt::adapt(rect().center());
          _port->setPos(_center + gex::qt::adapt(_t));
          _srcDelta += _srcSectionAngle;
        }
      
        QGraphicsItem::update();
      }
    }
  }
}
