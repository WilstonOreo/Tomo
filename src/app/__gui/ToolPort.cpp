
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QGraphicsDropShadowEffect>

#include "widget/ToolchainEditor.h"
#include "widget/tool/Port.h"
#include "widget/item/Node.h"
#include "widget/item/Edge.h"
#include "widget/tool/Node.h"

namespace tomo
{
  namespace widget
  {
    namespace tool
    {
      Port::Port(
        Node* _parent,
        const std::string& _text) :
        parent_(_parent),
        text_(_text),
        hover_(false)
      {
        setFlag(ItemSendsGeometryChanges);
        setAcceptHoverEvents(true);
        setCacheMode(DeviceCoordinateCache);
        setZValue(3.0);
      }

      bool Port::valid() const
      {
        return parent();
      }

      QRectF Port::rect() const
      {
        return QRectF(0,0,radius(),radius());
      }

      void Port::adjust()
      {
        if (!valid()) return;
      }

      float Port::radius()
      {
        return 6.0;
      }

      QRectF Port::boundingRect() const
      {
        return QRectF(-radius(),-radius(),radius(),radius()).normalized();
      }

      bool Port::empty() const
      {
        return items().empty();
      }

      void Port::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
      {
        QPointF _pos = pos();
        QRadialGradient gradient(_pos, rect().width() / 2);
        bool _empty = empty();

        QRectF _rect = rect();
        QPointF _delta(_rect.width()/3.0, _rect.height()/3.0);

        if (_empty)
        {
          gradient.setCenter(-_delta);
          gradient.setFocalPoint(-_delta);
          gradient.setColorAt(1, dark());
          gradient.setColorAt(0, light());
        }
        else
        {
          int _hoverFactor = hover_ ? 120 : 0;
          gradient.setCenter(-_delta*2.0);
          gradient.setFocalPoint(-_delta*2.0);
          gradient.setColorAt(0, lightTaken().light(_hoverFactor));
          gradient.setColorAt(1, darkTaken().light(_hoverFactor));
        }
        painter->setBrush(gradient);
        painter->setPen(QPen(dark(), 0.01));
        painter->drawEllipse(_rect);
      }

      void Port::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
      {
        hover_ = true;
        QGraphicsItem::update();
      }

      void Port::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
      {
        hover_ = false;
        QGraphicsItem::update();
      }

      QPainterPath Port::shape() const
      {
        QPainterPath path;
        path.addEllipse(rect());
        return path;
      }


      QVariant Port::itemChange(GraphicsItemChange change, const QVariant &value)
      {
        switch (change)
        {
        case ItemPositionHasChanged:
        {
          for (auto _edge : edges_)
            _edge->adjust();
          break;
        }
        default:
          break;
        };

        return QGraphicsItem::itemChange(change, value);
      }

      SinglePortInterface::SinglePortInterface(item_type* _item) :
        item_(_item) {}

      void SinglePortInterface::disconnect()
      {
        item_ = nullptr;
      }

      void SinglePortInterface::connect(const items_type& _items)
      {
        if (_items.size() != 1) return;
        item_ = _items.front();
      }

      void SinglePortInterface::connect(item_type* _item)
      {
        item_ = _item;
      }

      SinglePortInterface::items_type SinglePortInterface::items() const
      {
        return item_ ? items_type( {item_}) : items_type();
      }

      SourcePort::SourcePort(Node* _node, const std::string& _text) :
        Port(_node,_text) {}
      SinkPort::SinkPort(Node* _node, const std::string& _text) :
        Port(_node,_text) {}

      SingleSourcePort::SingleSourcePort(Node* _node, const std::string& _text, item_type* _item) :
        SourcePort(_node,_text),
        SinglePortInterface(_item)
      {
        parent()->sourcePorts().push_back(this);
      }

      SingleSinkPort::SingleSinkPort(Node* _node, const std::string& _text, item_type* _item) :
        SinkPort(_node,_text),
        SinglePortInterface(_item)
      {
        parent()->sinkPorts().push_back(this);
      }

    }
  }
}
