#pragma once

#include <QGraphicsItem>
#include <tbd/property.h>
#include "GenericEdge.hpp"

namespace tomo
{
  namespace widget
  {
    class ToolNode;

    class ToolEdge :
      public GenericEdge<ToolNode,ToolNode>,
      public QGraphicsItem
    {
    public:
      typedef GenericEdge<ToolNode,ToolNode> base_type;

      ToolEdge(ToolNode*, ToolNode*);
      ~ToolEdge();
      void adjust();

      enum { Type = UserType + 2 };
      int type() const
      {
        return Type;
      }

    protected:
      QRectF boundingRect() const;
      void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    private:
      QPolygonF generateArrow() const;
      QPolygonF generateStrip() const;
    };
  }
}
