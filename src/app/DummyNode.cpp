#include "widget/DummyNode.h"

#include <QPainter>
#include <tomo/item/Interface.hpp>
#include <tomo/tool/Interface.hpp>
#include <tomo/device/Interface.hpp>

#include "widget/paint/shadows.hpp"
#include "widget/paint/ToolNode.hpp"
#include "widget/paint/DeviceNode.hpp"
#include "widget/paint/ItemNode.hpp"
#include "widget/Port.h"

namespace tomo
{
  namespace widget
  {
    DummyNode::DummyNode(tool::Library const* _library) :
      ToolchainItem(_library,nullptr),
      mode_(EMPTY),
      id_("")
    {
      setGraphicsEffect(paint::shadows());
    }

    DummyNode::DummyNode(tool::Library const* _library, std::string const& _id) :
      ToolchainItem(_library,nullptr),
      mode_(EMPTY),
      id_("")
    {
      setGraphicsEffect(paint::shadows());
      setup(_id);
    }

    DummyNode::~DummyNode()
    {
    }
      
    DummyNode::elements_type DummyNode::elements(bool _recursive) const
    {
      return elements_type();
    }
     
    bool DummyNode::valid() const
    {
      return true;
    }

    void DummyNode::setup(std::string const& _id, QPointF const& _pos)
    {
      setPos(_pos);
     
      mode_ = EMPTY;
      sourcePorts_.clear();
      sinkPorts_.clear();
      id_= base::ElementId(_id);
      if (item::Registry::exists(_id))
      {
        mode_ = ITEM;
        shape_ = paint::ItemNode::shape();
        //paint::ItemNode::ports(this,_id,sinkPorts_);
      } else
      if (tool::Registry::exists(_id))
      {
        mode_ = TOOL;
        shape_ = paint::ToolNode::shape();
        //paint::ToolNode::ports(this,_id,sourcePorts_,sinkPorts_);
      } else
      if (device::Registry::exists(_id))
      {
        mode_ = DEVICE;
        shape_ = paint::DeviceNode::shape();
        //paint::DeviceNode::ports(this,_id,sourcePorts_);
      }
      show();
    }

    QPainterPath DummyNode::shape() const
    {
      return shape_;
    }

    QRectF DummyNode::boundingRect() const
    {
      return shape_.boundingRect();
    }

    base::ElementId DummyNode::elementId() const
    {
      return id_;
    }

    void DummyNode::paint(QPainter *_painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
      auto&& _light = lightColor();
      auto&& _dark = darkColor();
      auto&& _font = fontColor();

      switch (mode_)
      {
      case TOOL:
        paint::ToolNode::paint(_painter,_light,_dark,_font,elementId());
        break;
      case ITEM:
        paint::ItemNode::paint(_painter,_light,_dark,_font,elementId());
        break;
      case DEVICE:
        paint::DeviceNode::paint(_painter,_light,_dark,_font,elementId());
        break;
      default:
        return;
      }
    }
  }
}
