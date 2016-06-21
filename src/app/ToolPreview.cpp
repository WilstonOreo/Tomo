
#include "widget/ToolPreview.h"

#include "widget/ToolNode.h"
#include "widget/ItemNode.h"
#include "widget/DeviceNode.h"

#include "widget/ToolLibraryTree.h"

namespace tomo
{
  namespace widget
  {
    ToolPreview::ToolPreview(QWidget* _parent) :
      library_(nullptr),
      elementNode_(nullptr),
      tool_(nullptr),
      device_(nullptr),
      item_(nullptr)
    {
      QGraphicsScene *scene = new QGraphicsScene(this);
      scene->setItemIndexMethod(QGraphicsScene::NoIndex);
      scene->setSceneRect(QRect(0,0,width(),height()));
      setScene(scene);
      setViewportUpdateMode(BoundingRectViewportUpdate);
      setRenderHint(QPainter::Antialiasing);
      setTransformationAnchor(AnchorUnderMouse);
      setResizeAnchor(AnchorViewCenter);
    
      setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    void ToolPreview::setItem(ToolLibraryItem const* _item)
    {
      if (elementNode_)
      {
        scene()->removeItem(elementNode_.get());
      }

      auto&& _itemType = _item->itemType();
      elementNode_.reset(nullptr);
      switch (_itemType)
      {
        case ToolLibraryItem::TOOL:
          tool_ = std::move(_item->tool());
          if (tool_)
            elementNode_.reset(new ToolNode(library_,tool_.get()));
          break;
        case ToolLibraryItem::ITEM:
          item_ = std::move(_item->item());
          if (item_)
            elementNode_.reset(new ItemNode(library_,item_.get()));
          break;
        case ToolLibraryItem::DEVICE:
          device_ = std::move(_item->device());
          if (device_)
            elementNode_.reset(new DeviceNode(library_,device_.get()));
          break;
        default: 
          break;
      }

      if (elementNode_)
      {
        elementNode_->update();
        scene()->setSceneRect(elementNode_->boundingRect());
        elementNode_->setPos(scene()->sceneRect().center() - elementNode_->boundingRect().center()); 
        scene()->addItem(elementNode_.get());
        update();
      }
    }
  
    void ToolPreview::drawBackground(QPainter *painter, const QRectF &rect)
    {
      painter->setBrush(QColor("#201F1F"));
      painter->drawRect(rect);
    }

    void ToolPreview::resizeEvent(QResizeEvent* _event)
    {
      scene()->setSceneRect(QRect(0,0,width(),height()));
      if (elementNode_)
        elementNode_->setPos(scene()->sceneRect().center() - elementNode_->boundingRect().center()); 
    }
  }
}

