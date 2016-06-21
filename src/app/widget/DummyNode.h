#pragma once

#include "ToolchainItem.h"
#include <tomo/base/Id.hpp>

namespace tomo
{
  namespace widget
  {
    class SourcePort;
    class SinkPort;
  }

  namespace widget
  {
    /// Class for drawing a dummy element node with a given typeid   
    class DummyNode : public ToolchainItem 
    {
    public:
      enum Mode { EMPTY, TOOL, ITEM, DEVICE };

      typedef std::set<SourcePort*> sourceport_list;
      typedef std::set<SinkPort*> sinkport_list;

      DummyNode(tool::Library const* _library);
      DummyNode(tool::Library const*,std::string const& _id);
      ~DummyNode();

      void setup(std::string const& _id, QPointF const& _pos = QPointF(0,0));
      QRectF boundingRect() const;
      QPainterPath shape() const;

      base::ElementId elementId() const;
      elements_type elements(bool _recursive = false) const;
     
      bool valid() const;

      TBD_PROPERTY_REF_RO(sourceport_list,sourcePorts)
      TBD_PROPERTY_REF_RO(sinkport_list,sinkPorts)
      TBD_PROPERTY_RO(Mode,mode)
      TBD_PROPERTY_REF(QPointF,direction)
    protected:
      void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
    private:
      base::ElementId id_;
      QPainterPath shape_;
    };
  }
}

