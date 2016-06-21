#include "widget/Connection.h"
#include "widget/Port.h"
#include "widget/ToolNode.h"
#include "widget/paint/ToolNode.hpp"
#include "widget/ToolchainItem.h"
#include "widget/ToolchainEditor.h"

#include <tomo/tool/Connection.hpp>
#include <gex/qt.hpp>

namespace tomo
{
  namespace widget
  {
    Connection::Connection(
        widget::Port* _source,
        widget::Port* _dest,
        tool::Connection const* _connection) : 
      ToolchainItem(
          _source ? _source->library() : 
          _dest ? _dest->library() : nullptr,nullptr),
      edge_type(_source,_dest),
      connection_(_connection)
    {
      setFlag(ItemIsSelectable,true);
      if (source() && dest())
      {
        setZValue(std::max(source()->parentNode()->zValue(),dest()->parentNode()->zValue())+1.0);
        source()->connection(this);
        dest()->connection(this);
      }
      else 
      if (source())
      {
        setZValue(source()->zValue()+1.0);
        source()->connection(this);
      }
      else
      if (dest())
      {
        setZValue(dest()->zValue()+1.0);
        dest()->connection(this);
      }
      else
      {
        hide();
      }

      setToolTip(QString().fromStdString(elementId().str()));
      adjust();
    }

    Connection::~Connection()
    {
      if (source())
      {
        source()->connection(nullptr);
      }
      if (dest())
      {
        dest()->connection(nullptr);
      }
    }
 
    void Connection::update()
    {
      if (source()) source()->update();
    if (dest()) dest()->update();
      QGraphicsItem::update();
    }
      
    std::string Connection::connectionId() const
    {
      if (!source() || !dest()) return std::string();
      if (source()->portType() == dest()->portType()) return std::string();
      
      auto _source = source();
      auto _sink = dest();
      if (_source->portType() == widget::Port::SINK ||
          _sink->portType() == widget::Port::SOURCE)
      {
        std::swap(_source,_sink);
      }
      return _sink->fullId() + tool::Connection::connectionSeparator() + _source->fullId();
    }

    void Connection::adjust()
    {
      float r = Port::radius() * 0.5;

      edge_type::setPoints(
          source() ? mapFromItem(source(),r,r) : 
            gex::qt::adapt(sourcePoint()),
          dest() ? mapFromItem(dest(),r,r) : 
            gex::qt::adapt(destPoint()));

      prepareGeometryChange();
    }
      
    QRectF Connection::boundingRect() const
    {
      float adjust = Port::radius();
      return path().boundingRect().adjusted(-adjust,-adjust,adjust,adjust);
    }

    QPainterPath Connection::path() const
    {
      QPainterPath _path;
      float _dist = std::min(distance(),paint::ToolNode::radius());
    
      QPointF _c1 = source() ? _dist * source()->direction() : QPointF(0,0);
      QPointF _c2 = dest() ? -_dist * dest()->direction() : QPointF(0,0);
      
      auto&& _sourcePoint = gex::qt::adapt(sourcePoint());
      auto&& _destPoint = gex::qt::adapt(destPoint());
      _path.moveTo(_sourcePoint);
      _path.cubicTo(_sourcePoint - _c1,_destPoint + _c2,_destPoint);
      return _path;
    }

    void Connection::paint(QPainter *_painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
      if (!_painter->isActive()) return;
      
      auto _port = source() ? source() : dest();
      if (!_port) return;

      auto&& _light = _port->lightColor();
      auto&& _dark = _port->darkColor();

      auto&& _path = path();
      QColor _strokeColor = isSelected() ? QColor(255,255,255,128) : QColor(0,0,0,255);

      _painter->setPen(QPen(_strokeColor,4.0));
      _painter->setBrush(Qt::NoBrush);
      _painter->drawPath(_path);
      _painter->setPen(QPen(isSelected() || hovered() ? _light : _dark,1.5));
      _painter->drawPath(_path);
      if (source()) drawNode(_painter,_light,_dark, gex::qt::adapt(sourcePoint()));
      if (dest()) drawNode(_painter,_light,_dark,gex::qt::adapt(destPoint()));
    }
      
    bool Connection::valid() const
    {
      return edge_type::valid();
    }
     
    void Connection::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
    {
      QGraphicsItem::mouseMoveEvent(event);
    }

    void Connection::mousePressEvent ( QGraphicsSceneMouseEvent * event )
    {
      QGraphicsItem::mousePressEvent(event);

    }
    void Connection::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
    {
      QGraphicsItem::mouseReleaseEvent(event);
    }

    base::ElementId Connection::elementId() const
    {
      return source() ? source()->elementId() : 
        dest() ? dest()->elementId() : base::ElementId("");
    }
    
    Connection::elements_type Connection::elements(bool _recursive) const
    { 
      return source() ? source()->elements() : 
        dest() ? dest()->elements() : elements_type();
    }

    QPainterPath Connection::shape() const
    {
      QPainterPath&& _path = path(); 
      /// Make the shape more larger for easier mouse handling
      QPainterPathStroker stroker;
      float r = Port::radius()*0.5;
      stroker.setWidth(r);
      stroker.setJoinStyle(Qt::MiterJoin); // and other adjustments you need
      QPainterPath _newPath = (stroker.createStroke(_path) + _path).simplified(); 

      auto&& _sourcePoint = gex::qt::adapt(sourcePoint());
      auto&& _destPoint = gex::qt::adapt(destPoint());
      _newPath.addEllipse(QRectF(_sourcePoint,_sourcePoint).adjusted(-r-2,-r-2,r+2,r+2));
      _newPath.addEllipse(QRectF(_destPoint,_destPoint).adjusted(-r-2,-r-2,r+2,r+2));
      return _newPath;
    }

    void Connection::drawNode(
        QPainter* _painter, 
        QColor const& _light, 
        QColor const& _dark,
        QPointF const& _pos) const
    {
      float r = Port::radius()*0.5;
      QRectF _rect(_pos,_pos);
      _rect.adjust(-r-2,-r-2,r+2,r+2);
      QPointF _delta(_rect.width()/4.0,_rect.height()/4.0);
      
      QRadialGradient _gradient(_rect.center() - _delta,r);
      _gradient.setColorAt(0,_light);
      _gradient.setColorAt(1,_dark);
      _painter->setBrush(_gradient);
      
      QColor _strokeColor = isSelected() ? QColor(255,255,255,128) : QColor(0,0,0,128);
      _painter->setPen(QPen(_strokeColor,1.0));
      _painter->drawEllipse(_rect);
    }

  }
}
