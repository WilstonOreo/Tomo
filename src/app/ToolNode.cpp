
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <tbd/boolean.h>
#include <gex/qt.hpp>
#include <gex/algorithm/distance.hpp>
#include <tomo/tool/Interface.hpp>
#include <tomo/tool/Chain.hpp>


#include "widget/ToolEdge.h"
#include "widget/Connection.h"
#include "widget/ToolNode.h"
#include "widget/Port.h"
#include "widget/ToolchainEditor.h"

#include "widget/paint/ToolNode.hpp"

namespace tomo
{
  namespace widget
  {
    ToolNode::ToolNode(
        tool::Library const* _library,
        tool_interface_type* _tool) :
      base_type(_library),
      tool_(_tool),
      prev_(nullptr),
      prevTool_(nullptr),
      next_(nullptr),
      nextTool_(nullptr),
      moving_(false)
    {
      buildPorts();
    }
    
    ToolNode::ToolNode(
      ToolchainEditor* _parent, 
      base::Id const& _id,
      tool_interface_type* _tool) :
      base_type(_parent,_id),
      tool_(_tool),
      prev_(nullptr),
      prevTool_(nullptr),
      next_(nullptr),
      nextTool_(nullptr),
      moving_(false)
    {
      buildPorts();
    }

    ToolNode::~ToolNode()
    {
      if (prev_)
        prev_->dest(nullptr);
      if (next_)
        next_->source(nullptr);
      if (nextTool_)
        nextTool_->prevTool(nullptr);
      if (prevTool_)
        prevTool_->nextTool(nullptr);
    }

    QRectF ToolNode::boundingRect() const
    {
      qreal adjust = 2;
      QRectF _boundingRect = paint::ToolNode::rect();
      _boundingRect.adjust(-adjust,-adjust,adjust,adjust);
      for (auto& _source : sourcePorts())
        _boundingRect |= _source->boundingRect();
      for (auto& _sink : sinkPorts())
        _boundingRect |= _sink->boundingRect();

      return _boundingRect;
    }

    base::ElementId ToolNode::elementId() const
    {
      return tool_->elementId();
    }

    ToolNode::elements_type ToolNode::elements(bool _recursive) const
    {
      if (!valid() || !tool_)      
        return elements_type({tool_});
        
      using tbd::boolean::operator|=;
      elements_type _elements({tool_});
      if (!_recursive) return _elements;
      auto&& _sinkPorts = sinkPorts();
      for (auto& _port : _sinkPorts)
      {
        _elements |= _port->elements();
      }
      return _elements;
    }

    void ToolNode::buildPorts()
    {
      if (!tool_) return;
      
      auto&& _sources = tool_->sources();
      for (auto& _source : _sources)
      {
        addSourcePort(new widget::SourcePort(this,_source.first,_source.second));
      }
      
      auto&& _sinks = tool_->sinks();
      for (auto& _sink : _sinks)
      {
        addSinkPort(new widget::SinkPort(this,_sink.first,_sink.second));
      }
    
      update();
    }

    bool ToolNode::ready() const
    {
      return true;
    }

    bool ToolNode::valid() const
    {
      if (!editor()) return true;
      if (!tool_) return false;
      return tool_->valid();
    }

    void ToolNode::paint(QPainter *_painter, const QStyleOptionGraphicsItem *option, QWidget *)
    {
      if (!_painter->isActive()) return; 

      paint::ToolNode::paint(_painter,
          lightColor(),darkColor(),fontColor(),
          elementId(),id(),
          valid(),isSelected());
    }
      
    QPainterPath ToolNode::shape() const
    {
      return paint::ToolNode::shape();
    }

    QVariant ToolNode::itemChange(GraphicsItemChange change, const QVariant &value)
    {
      if (!editor()) return QGraphicsItem::itemChange(change, value);
      
      switch (change)
      {
      case ItemPositionHasChanged:
        if (prevTool_) prevTool_->update();
        if (nextTool_) nextTool_->update();
        if (prev_) prev_->adjust();
        if (next_) next_->adjust();
        update();
        break;
      case ItemPositionChange:
        /*
      {
        float _minDist = std::numeric_limits<float>::max();
        widget::ToolNode const* _minTool = nullptr;

        auto&& _rect = paint::ToolNode::rect();
        auto&& _center = _rect.center();
        auto&& r = paint::ToolNode::radius();
        
        for (auto& _tool : editor()->toolNodes())
        {
          if (_tool == this) continue; 
          auto&& p = mapFromItem(_tool,_rect.center());
          auto&& _dist = gex::distance(gex::qt::adapt(p),gex::qt::adapt(_center));
          if (_dist < _minDist)
          {
            _minDist = _dist;
            _minTool = _tool;
          }
        }
        if (_minTool)
        {
          auto&& p = mapFromItem(_minTool,_rect.center());
          auto&& _dist = gex::distance(gex::qt::adapt(p),gex::qt::adapt(_center));
          if (_dist < r)
          {
            auto&& dir = gex::qt::adapt(_center-p).normalized();
            if (prevTool_) prevTool_->update();
            if (nextTool_) nextTool_->update();
            if (prev_) prev_->adjust();
            if (next_) next_->adjust(); 
            return _minTool->pos() + gex::qt::adapt(gex::Point2(r*1.01*dir));
          }
        }
        break;
      }*/
      default:
        break;
      };

      return ElementNode::itemChange(change, value);
    }

    void ToolNode::update()
    {
       gex::Range _srcDegRange(0,180);
       gex::Scalar _srcSectionAngle = _srcDegRange.length() / sourcePorts().size();
       gex::Scalar _srcDelta = 90 + _srcDegRange.min() + _srcSectionAngle * 0.5;
       
       setDirection();
        if (prev_) prev_->update();
        if (next_) next_->update();

       auto&& _dir = gex::qt::adapt(direction());

       QPointF _center = -QPoint(Port::rect().width()*0.5,Port::rect().height()*0.5);

       auto&& r = paint::ToolNode::radius(); 
       auto&& _rect = paint::ToolNode::rect();

       for (auto _port : sourcePorts())
       {
         gex::Point2 _t = Eigen::Rotation2D<gex::Scalar>(gex::deg2rad(_srcDelta)) * _dir * r*0.4 + gex::qt::adapt(_rect.center());
         _port->setPos(_center + gex::qt::adapt(_t));
          _port->adjust();
         _srcDelta += _srcSectionAngle;
       }

       _srcSectionAngle = _srcDegRange.length() / sinkPorts().size();
       _srcDelta = 90 + _srcDegRange.max() + _srcSectionAngle * 0.5;

       for (auto _port : sinkPorts())
       {
         gex::Point2 _t = Eigen::Rotation2D<gex::Scalar>(gex::deg2rad(_srcDelta)) * _dir * r*0.4 + gex::qt::adapt(_rect.center());
         _port->setPos(_center + gex::qt::adapt(_t));
          _port->adjust();
         _srcDelta += _srcSectionAngle;
       }
     
      QGraphicsItem::update();
    }
    
    void ToolNode::setDirection()
    {
      if (!editor())
      {
        direction() = QPointF(1.0,0.0);
        return;
      }

      if (prev_ && next_)
      {
        direction() = gex::qt::adapt(gex::Point2(prev_->dir() + next_->dir()).normalized());
      } else
      if (next_)
      {
        direction() = gex::qt::adapt(next_->dir());

      } else
      if (prev_)
      {
        direction() = gex::qt::adapt(prev_->dir());
      } else
        direction() = QPointF(0.0,1.0);
    }
  }
}
