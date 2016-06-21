#pragma once

#include <QPainter>
#include <QGraphicsItem>

#include "ToolchainItem.h"

class QGraphicsSceneHoverEvent;

namespace tomo
{
  namespace item
  {
    class PortInterface;
    class SinkInterface;
    class SourceInterface;
  }

  namespace widget
  {
    class ElementNode;
    class Connection;

    class Port : public ToolchainItem
    {
    public:
      enum PortType { SOURCE, SINK };

      Port(ElementNode*,std::string const& _id,item::PortInterface const*);

      static QRectF rect();
      static float radius();

      virtual PortType portType() const = 0;
      bool valid() const;

      base::ElementId elementId() const;
      elements_type elements(bool _recursive = false) const;

      void adjust();
      QPointF direction() const;

      bool drawCaption() const;
      QRectF boundingRect() const;

      std::string fullId() const;


      TBD_PROPERTY_RO(ElementNode*,parentNode)
      TBD_PROPERTY(bool,disabled)
      TBD_PROPERTY(bool,highlighted)
      TBD_PROPERTY(Connection*,connection)
      TBD_PROPERTY_REF(std::string,id)
      TBD_PROPERTY_RO(item::PortInterface const*,port)
    protected:
      QRectF captionRect() const;
      QPainterPath shape() const;
      void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
   //   QVariant itemChange(GraphicsItemChange change, const QVariant &value);

      void hoverEnterEvent( QGraphicsSceneHoverEvent * event );
      void hoverLeaveEvent( QGraphicsSceneHoverEvent * event );
    };

    class SourcePort : public Port
    {
    public:
      SourcePort(ElementNode*,std::string const& _id,item::SourceInterface const*);
      inline PortType portType() const
      {
        return SOURCE;
      }
    };

    class SinkPort : public Port
    {
    public:
      SinkPort(ElementNode*,std::string const& _id,item::SinkInterface const*);

      inline PortType portType() const
      {
        return SINK;
      }
    };
  }
}
