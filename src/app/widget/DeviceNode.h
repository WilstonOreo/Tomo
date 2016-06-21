#pragma once

#include "ElementNode.h"

namespace tomo
{
  namespace device
  {
    class Interface;
  }

  namespace widget
  {
    class DeviceNode : public ElementNode
    {
    public:
      typedef ElementNode base_type;
      typedef tomo::device::Interface device_interface_type;

      DeviceNode(tool::Library const*,device_interface_type*);
      DeviceNode(ToolchainEditor*_parent,base::Id const&, device_interface_type*);

      QRectF boundingRect() const;
      QPainterPath shape() const;
      void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
      static QRectF rect();
      static float radius();

      void update();
      bool valid() const;

      base::ElementId elementId() const;
      std::set<base::Element const*> elements(bool _recursive) const;

      TBD_PROPERTY_RO(device_interface_type*,device)
    protected:
      QVariant itemChange(GraphicsItemChange change, const QVariant &value);
      void mousePressEvent(QGraphicsSceneMouseEvent*);
      void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
      void mouseMoveEvent(QGraphicsSceneMouseEvent*);
      void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*);
    private:
      void toggleOpen();

      void buildPorts();
      QGraphicsProxyWidget* dialogProxy_;
    };
  }
}
