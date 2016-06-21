#include "widget/tool/Edge.h"
#include "widget/tool/Node.h"

#include <QPainter>
#include <gex/qt.hpp>
#include <math.h>

namespace tomo
{
  namespace widget
  {
    namespace tool
    {
      Edge::Edge(Node* _prev, Node *_next) : 
        base_type(_prev,_next)
      {
        setAcceptedMouseButtons(0);
        source()->next(this);
        dest()->prev(this);
        adjust();
      }

      void Edge::adjust()
      {
        if (!valid()) return;
        auto _r = Node::radius();
        QLineF _line(mapFromItem(source(), -_r/2, -_r/2), mapFromItem(dest(), -_r/2, -_r/2));
        prepareGeometryChange();
        base_type::setPoints(_line.p1(),_line.p2());
      }

      QRectF Edge::boundingRect() const
      {
        return gex::qt::adapt(base_type::boundingRect());
      }

      QPolygonF Edge::generateStrip() const
      {
        gex::Point2 _n = normal() * Node::radius() * 0.5;
        QPointF _qdelta = gex::qt::adapt(gex::Point2(_n));
        auto _src = gex::qt::adapt(sourcePoint());
        auto _dest = gex::qt::adapt(destPoint());

        return QPolygonF() 
          << QPointF(_src - _qdelta)
          << QPointF(_src + _qdelta)
          << QPointF(_dest + _qdelta)
          << QPointF(_dest - _qdelta);
      }
      
      QPolygonF Edge::generateArrow() const
      {  
        auto _dist = distance();
        auto _r = Node::radius();
        if (_dist < _r) return QPolygonF();
        gex::Point2 _d = dir();
        gex::Point2 _n = normal() * Node::radius();
        QPointF _top = gex::qt::adapt(gex::Point2(_d * (_dist - _r/2.0)));
        QPointF _side = gex::qt::adapt(gex::Point2(_d * (_dist - _r)));
        QPointF _qdelta = gex::qt::adapt(gex::Point2(_n * 0.5));
        QPointF _adelta = gex::qt::adapt(gex::Point2(_n * 0.25)); 
        auto _src = gex::qt::adapt(sourcePoint());
        return QPolygonF()
                             << QPointF(_src - _adelta)
                             << QPointF(_src + _adelta)
                             << QPointF(_src + _side + _adelta)
                             << QPointF(_src + _side + _qdelta)
                             << QPointF(_src + _top)
                             << QPointF(_src + _side - _qdelta)
                             << QPointF(_src + _side - _adelta);
      }

      void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
      {
        if (!valid()) return;

        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(strip()));
        painter->drawPolygon(generateStrip());

        painter->setBrush(QBrush(arrow()));
        painter->drawPolygon(generateArrow());
      }
    }
  }
}
