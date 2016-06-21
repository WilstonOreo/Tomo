#pragma once

#include "GenericEdge.hpp"
#include "ToolchainItem.h"

namespace tomo
{
  namespace tool
  {
    struct Connection;
  }

  namespace widget
  {
    class Port;

    class Connection : 
      public ToolchainItem,
      public GenericEdge<widget::Port,widget::Port>
    {
    public:
      typedef GenericEdge<widget::Port,widget::Port> edge_type;
      
      Connection(
          widget::Port* _source,
          widget::Port* _dest,
          tool::Connection const* _connection);
      ~Connection();

      bool valid() const;

      base::ElementId elementId() const;
      elements_type elements(bool _recursive = false) const;

      std::string connectionId() const;

      void adjust();
      QRectF boundingRect() const;

      void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
      QPainterPath shape() const;

      void update();

      TBD_PROPERTY_RO(tool::Connection const*,connection)
    protected:
      void 	mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
      void 	mousePressEvent ( QGraphicsSceneMouseEvent * event );
      void 	mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ); 

    private:
      QPainterPath path() const;
      void drawNode(
          QPainter* _painter,   
          QColor const& _light, 
          QColor const& _dark,
          QPointF const& _pos) const;
    };
  }
}

    
