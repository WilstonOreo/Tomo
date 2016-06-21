#include "widget/ElementNode.h"

#include <QGraphicsSceneMouseEvent>
#include "widget/ToolchainEditor.h"
#include "widget/Port.h"
#include "widget/Connection.h"
#include "widget/paint/shadows.hpp"

namespace tomo
{
  namespace widget
  {
    ElementNode::ElementNode(ToolchainEditor* _editor, base::Id const& _id) : 
      ToolchainItem(_editor,nullptr),
      id_(_id),
      portHovered_(nullptr),
      direction_(0,1)
    {
      setFlag(ItemIsMovable,true);
      setZValue(2.0);
      setGraphicsEffect(paint::shadows());
    }
    
    ElementNode::ElementNode(tool::Library const* _library) : 
      ToolchainItem(_library,nullptr),
      portHovered_(nullptr),
      direction_(0,1)
    {
      setGraphicsEffect(paint::shadows());
    }  

    ElementNode::~ElementNode()
    {
    }

    void ElementNode::addSourcePort(SourcePort* _port)
    {
      sourcePorts_.insert(_port);
    }
       
    QVariant ElementNode::itemChange(GraphicsItemChange change, const QVariant &value)
    {
      if (!editor()) return QGraphicsItem::itemChange(change, value);

      switch (change)
      {
      case ItemPositionChange:
        if (portHovered())
        {
          return pos();
        }
        
        break;
      default:
        break;
      };

      return QGraphicsItem::itemChange(change, value);
    }
   
    void ElementNode::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
    {
      hovered(true);
      update();
      QGraphicsItem::hoverEnterEvent(event);
    }
    
    void ElementNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
    {
      hovered(false);
      update();
      QGraphicsItem::hoverLeaveEvent(event);
    }
      
    void ElementNode::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
    {
      QGraphicsItem::mouseMoveEvent(event);
    }

    void ElementNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
    {
      QGraphicsItem::mousePressEvent(event);
    }

    void ElementNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
    {
      QGraphicsItem::mouseReleaseEvent(event);
    }

    void ElementNode::addSinkPort(SinkPort* _port)
    {
      sinkPorts_.insert(_port);
    }

    void ElementNode::disconnect()
    {
        for (auto& _port : sourcePorts())
        {
          if (_port->connection() && editor())
          {
            editor()->deleteConnection(_port->connection());
          }
        }
        for (auto& _port : sinkPorts())
        {
          if (_port->connection() && editor())
          {
            editor()->deleteConnection(_port->connection());
          }
        }
    }
  }
}
