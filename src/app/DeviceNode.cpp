
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QFileDialog>

#include <tbd/boolean.h>
#include <gex/qt.hpp>
#include <tomo/device/Interface.hpp>

#include "widget/DeviceNode.h"
#include "widget/Port.h"
#include "widget/ToolchainEditor.h"

#include "widget/paint/DeviceNode.hpp"

namespace tomo
{
  namespace widget
  {
    DeviceNode::DeviceNode(
        tool::Library const* _library,
        device_interface_type* _device) :
      base_type(_library),
      device_(_device)
    {
      buildPorts();
    }

    DeviceNode::DeviceNode(
      ToolchainEditor* _parent, 
      base::Id const& _id,
      device_interface_type* _device) :
      base_type(_parent,_id),
      device_(_device)
    {
      buildPorts();
    }

    QRectF DeviceNode::boundingRect() const
    {
      return paint::DeviceNode::boundingRect();
    }

    base::ElementId DeviceNode::elementId() const
    {
      return device_->elementId();
    }

    void DeviceNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent*)
    {
      if (!device_ || !editor()) return;
      if (!device_->valid()) return;

      QFileDialog _dialog(
          static_cast<QWidget*>(editor()->parent()),
          "Save GCode File...",
          ".",
          "GCode (*.gcode)");
      _dialog.setAcceptMode(QFileDialog::AcceptSave);
      if (!_dialog.exec()) return;
    
      auto&& _files = _dialog.selectedFiles();
      if (_files.empty()) return; 
      QString _qfilename = _files[0];
      if( _qfilename.isEmpty() ) return;

      if( _qfilename.isEmpty() ) return;

      auto&& _filename = _qfilename.toStdString();
      device_->saveToFile(_filename);
    }

    DeviceNode::elements_type DeviceNode::elements(bool _recursive) const
    {
      if (!valid() || !device_)
        return elements_type({device_});
      using tbd::boolean::operator|=;
      elements_type _elements({device_});
      if (!_recursive) return _elements;
      auto&& _ports = sourcePorts();
      for (auto& _port : _ports)
      {
        _elements |= _port->elements();
      }
      return _elements;
    }

    void DeviceNode::buildPorts()
    {
      if (!device_) return;
      
      auto&& _sources = device_->sources();
      for (auto& _source : _sources)
      {
        addSourcePort(new widget::SourcePort(this,_source.first,_source.second));
      }

      update();
    }

    QPainterPath DeviceNode::shape() const
    {
      return paint::DeviceNode::shape();
    }

    bool DeviceNode::valid() const
    {
      if (!device_) return false;
      return device_->valid();
    }

    void DeviceNode::paint(QPainter *_painter, const QStyleOptionGraphicsItem *option, QWidget *)
    {
      if (!_painter->isActive()) return;
      paint::DeviceNode::paint(
          _painter,
          lightColor(),darkColor(),fontColor(),
          elementId(),id(),
          valid(),isSelected());
    }

    QVariant DeviceNode::itemChange(GraphicsItemChange change, const QVariant &value)
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

    void DeviceNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
      ElementNode::mousePressEvent(event);
    }

    void DeviceNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
    {
      ElementNode::mouseReleaseEvent(event);
    }

    void DeviceNode::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
    {
      ElementNode::mouseMoveEvent(event);
    }

    void DeviceNode::update()
    {
      auto&& _rect = shape().boundingRect();
      auto&& _ports = sourcePorts();
      float _r = Port::radius() * 1.5;
      float _delta = (_rect.width() - _r)/ _ports.size();
      float _posX = _delta / 2.0;
      for (auto _port : _ports)
      {
        _port->setPos(_posX,_r*0.3);
        _port->adjust();
        _posX += _delta;
      }

      QGraphicsItem::update();
    }
  }
}
