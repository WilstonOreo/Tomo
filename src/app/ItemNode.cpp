
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>

#include <gex/qt.hpp>
#include <tomo/item/Interface.hpp>

#include "widget/ItemNode.h"
#include "widget/Port.h"
#include "widget/ToolchainEditor.h"
#include "widget/paint/ItemNode.hpp"

#include <QFileDialog>

namespace tomo
{
  namespace widget
  {
    ItemNode::ItemNode(
        tool::Library const* _library,
        item_interface_type* _item) :
      base_type(_library),
      item_(_item)
    {
      buildPorts();
    }
 
    ItemNode::ItemNode(
      ToolchainEditor* _parent, 
      base::Id const& _id,
      item_interface_type* _item) :
      base_type(_parent,_id),
      item_(_item)
    {
      buildPorts();
    }

    QRectF ItemNode::boundingRect() const
    {
      qreal adjust = 20;
      QRectF _boundingRect = shape().boundingRect();
      _boundingRect.adjust(-adjust,-adjust,adjust,adjust);
      return _boundingRect;
    }

    base::ElementId ItemNode::elementId() const
    {
      return item_->elementId();
    }

    ItemNode::elements_type ItemNode::elements(bool _recursive) const
    {
      return elements_type( {item_});
    }
      
    void ItemNode::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
    {
      ElementNode::mouseMoveEvent(event);
    }

    void ItemNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
    {
      ElementNode::mousePressEvent(event);
    }

    void ItemNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
    {
      ElementNode::mouseReleaseEvent(event);
    }

    void ItemNode::buildPorts()
    {
      if (!item_) return;

      auto&& _sink = item_->getSink();
      addSinkPort(new widget::SinkPort(this,id(),_sink));
      
      auto _port = *sinkPorts().begin();

      update();
      auto&& _rect = shape().boundingRect();
      float _r = Port::radius() * 1.5;
      _port->setPos(_rect.width()-paint::ItemNode::shift() - _r,paint::ItemNode::radius()*0.5 - _r);
    }

    QPainterPath ItemNode::shape() const
    {
      return paint::ItemNode::shape();
    }

    bool ItemNode::valid() const
    {
      return item_;
    }
      
    void ItemNode::openFile()
    {
      if (!item_ || !editor()) return;
      if (!item_->fileSource()) return;

      QFileDialog _dialog(
          static_cast<QWidget*>(editor()->parent()),
          "Open Mesh File...",
          ".",
          "STL Meshes (*.stl)");

      if (!_dialog.exec()) return;
    
      auto&& _files = _dialog.selectedFiles();
      if (_files.empty()) return;
 
      QString _qfilename = _files[0];
      if( _qfilename.isEmpty() ) return;

      auto&& _filename = _qfilename.toStdString();
      item_->fileSource()->load(_filename);
    }

    void ItemNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent*)
    {
      openFile();
    }

    void ItemNode::paint(QPainter *_painter, const QStyleOptionGraphicsItem *option, QWidget *)
    {
      if (!_painter->isActive()) return;
      paint::ItemNode::paint(_painter,
          lightColor(),darkColor(),fontColor(),
          elementId(),id(),isSelected());
    }

    QVariant ItemNode::itemChange(GraphicsItemChange change, const QVariant &value)
    {
      switch (change)
      {
        case ItemPositionHasChanged:
        {
          update();
        }
        default: break;
      }

      return ElementNode::itemChange(change, value);
    }

    void ItemNode::update()
    {
      if (sinkPorts().empty()) return;

      auto _port = *sinkPorts().begin();
      auto&& _rect = shape().boundingRect();
      float _r = Port::radius() * 1.5;
      _port->setPos(_rect.width()-paint::ItemNode::shift() - _r,paint::ItemNode::radius()*0.5 - _r);
      QGraphicsItem::update();
    }
  }
}
