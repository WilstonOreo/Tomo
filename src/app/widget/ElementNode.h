#pragma once

#include <tbd/property.h>
#include <memory>
#include <QGraphicsItem>
#include <QList>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsProxyWidget>
#include "ToolchainItem.h"
#include "Connection.h"
#include <tomo/base/Id.hpp>

class QGraphicsSceneMouseEvent;

namespace tomo
{
  namespace widget
  {
    class Port;
    class SourcePort;
    class SinkPort;


    class ElementNode : public ToolchainItem 
    {
    public:
      typedef std::set<SourcePort*> sourceport_list;
      typedef std::set<SinkPort*> sinkport_list;

      ElementNode(tool::Library const* _library);
      ElementNode(ToolchainEditor*,base::Id const&);
      ~ElementNode();

      void disconnect();
      inline virtual bool prefix() const { return true; }  

      TBD_PROPERTY_REF_RO(sourceport_list,sourcePorts)
      TBD_PROPERTY_REF_RO(sinkport_list,sinkPorts)
      TBD_PROPERTY_REF_RO(base::Id,id)
      TBD_PROPERTY(Port*,portHovered)
      TBD_PROPERTY_REF(QPointF,direction)
    protected:

      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent*);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent*);
      virtual void mousePressEvent(QGraphicsSceneMouseEvent*);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
      void addSourcePort(SourcePort* _sourcePort);
      void addSinkPort(SinkPort* _sinkPort);
      QVariant itemChange(GraphicsItemChange change, const QVariant &value);
 
    private:
      virtual void buildPorts() = 0;  

    };
  }
}
