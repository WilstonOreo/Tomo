#pragma once

#include "ElementNode.h"

namespace tomo
{
  namespace item
  {
    class Interface;
  }

  namespace widget
  {
    class ItemNode : public ElementNode
    {
    public:
      typedef ElementNode base_type;
      typedef tomo::item::Interface item_interface_type;

      ItemNode(tool::Library const*_library,item_interface_type*);
      ItemNode(ToolchainEditor*_parent,base::Id const&, item_interface_type*);

      QRectF boundingRect() const;
      QPainterPath shape() const;
      void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
      static QRectF rect();
      void update();
      bool valid() const;
      bool prefix() const { return false; }  

      void openFile();

      base::ElementId elementId() const;
      elements_type elements(bool _recursive = false) const;

      TBD_PROPERTY_RO(item_interface_type*,item)
    protected:
      QVariant itemChange(GraphicsItemChange change, const QVariant &value);
      void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*);
      void mouseMoveEvent(QGraphicsSceneMouseEvent*);
      void mousePressEvent(QGraphicsSceneMouseEvent*);
      void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
    private:
      void toggleOpen();

      void buildPorts();
      QGraphicsProxyWidget* dialogProxy_;
    };
  }
}
