#include "widget/ToolEdge.h"
#include "widget/ToolNode.h"

#include <QPainter>
#include <gex/qt.hpp>

#include "widget/paint/ToolNode.hpp"
#include <math.h>

namespace tomo
{
  namespace widget
  {
    ToolEdge::ToolEdge(ToolNode* _prev, ToolNode *_next) :
      base_type(_prev,_next)
    {
      setCacheMode(NoCache);
      setZValue(0.0);
      setAcceptedMouseButtons(0);
      source()->next(this);
      dest()->prev(this);
      adjust();
    }

    ToolEdge::~ToolEdge()
    {
      if (source())
        source()->next(nullptr);
      if (dest())
        dest()->prev(nullptr);
    }

    void ToolEdge::adjust()
    {
      if (!valid()) return;
      
      auto _r = paint::ToolNode::radius();
      QLineF _line(mapFromItem(source(), -_r/2, -_r/2), mapFromItem(dest(), -_r/2, -_r/2));
      base_type::setPoints(_line.p1(),_line.p2());
      prepareGeometryChange();
    }

    QRectF ToolEdge::boundingRect() const
    {
      return gex::qt::adapt(base_type::boundingRect()).adjusted(-2,-2,2,2);
    }

    QPolygonF ToolEdge::generateStrip() const
    {
      auto _r = paint::ToolNode::radius();
      gex::Point2 _n = normal() * _r * 0.5;
      QPointF _qdelta = gex::qt::adapt(gex::Point2(_n));
      auto _src = gex::qt::adapt(sourcePoint());
      auto _dest = gex::qt::adapt(destPoint());

      return QPolygonF()
             << QPointF(_src - _qdelta)
             << QPointF(_src + _qdelta)
             << QPointF(_dest + _qdelta)
             << QPointF(_dest - _qdelta);
    }

    QPolygonF ToolEdge::generateArrow() const
    {
      auto _dist = distance();
      auto _r = paint::ToolNode::radius();
      if (_dist < _r) return QPolygonF();
      gex::Point2 _d = dir();
      gex::Point2 _n = normal() * _r;
      QPointF _top = gex::qt::adapt(gex::Point2(_d * (_dist - _r/2.0)));
      QPointF _side = gex::qt::adapt(gex::Point2(_d * (_dist - _r)));
      QPointF _qdelta = gex::qt::adapt(gex::Point2(_n * 0.5));
      QPointF _adelta = gex::qt::adapt(gex::Point2(_n * 0.25));
      auto _src = gex::qt::adapt(sourcePoint()) + QPoint(_r,_r);
      return QPolygonF()
             << QPointF(_src - _adelta)
             << QPointF(_src + _adelta)
             << QPointF(_src + _side + _adelta)
             << QPointF(_src + _side + _qdelta)
             << QPointF(_src + _top)
             << QPointF(_src + _side - _qdelta)
             << QPointF(_src + _side - _adelta);
    }

    void ToolEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
      if (!valid() || !painter->isActive()) return;

      auto&& _ellipse = mapFromItem(source(),source()->shape());

      QPainterPath _path;
      _path.addPolygon(generateArrow());
      _path -= _ellipse;

      painter->setPen(QColor(0,0,0,128));
      painter->setBrush(QBrush(QColor(128,128,128,32)));
      painter->drawPath(_path);
    }
  }
}
