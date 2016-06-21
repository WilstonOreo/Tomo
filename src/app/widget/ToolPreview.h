#pragma once

#include <memory>
#include <QGraphicsView>

#include "ElementNode.h"
#include <tomo/item/Interface.hpp>
#include <tomo/device/Interface.hpp>
#include <tomo/tool/Interface.hpp>

namespace tomo
{
  namespace tool
  {
    struct Library;
    class Interface;
  }

  namespace device
  {
    class Interface;
  }

  namespace item
  {
    class Interface;
  }
  class ToolLibraryItem;

  namespace widget
  {


    class ToolPreview : public QGraphicsView 
    {
      Q_OBJECT
    public:
      ToolPreview(QWidget* _parent = nullptr);

      void setItem(ToolLibraryItem const*_item);

      TBD_PROPERTY(tool::Library*,library)
    protected:
      void drawBackground(QPainter *painter, const QRectF &rect);
      void resizeEvent(QResizeEvent* _event);

    private:

      std::unique_ptr<ElementNode> elementNode_;
      std::unique_ptr<tool::Interface> tool_; 
      std::unique_ptr<device::Interface> device_;
      std::unique_ptr<item::Interface> item_;
    };
  }
}
