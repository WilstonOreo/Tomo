#pragma once

#include "ElementNode.h"

namespace tomo
{
  namespace tool
  {
    class Interface;
  }

  namespace widget
  {
    class ToolEdge;

    class ToolNode : public ElementNode
    {
    public:
      typedef ElementNode base_type;
      typedef tomo::tool::Interface tool_interface_type;

      ToolNode(tool::Library const*,tool_interface_type*);
      ToolNode(ToolchainEditor*_parent,base::Id const&, tool_interface_type*);
      ~ToolNode();

      QRectF boundingRect() const;
      void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

      QPainterPath shape() const;

      void update();
      bool valid() const;
      bool ready() const;

      base::ElementId elementId() const;
      std::set<base::Element const*> elements(bool _recursive) const;

      TBD_PROPERTY_RO(tool_interface_type*,tool)
      TBD_PROPERTY(ToolEdge*,prev)
      TBD_PROPERTY(ToolNode*,prevTool)
      TBD_PROPERTY(ToolEdge*,next)
      TBD_PROPERTY(ToolNode*,nextTool)
    protected:
      QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    private:
      void toggleOpen();
      void setDirection();

      bool moving_;
      void buildPorts();
    };
  }
}
