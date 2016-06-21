#include "widget/ToolchainEditor.h"
#include "widget/tool/Edge.h"
#include "widget/tool/Node.h"
#include "widget/tool/Port.h"
#include "widget/item/Edge.h"
#include "widget/item/Node.h"

#include <QtGui>

#include <math.h>

namespace tomo
{
  namespace widget
  {
    ToolchainEditor::ToolchainEditor(QWidget *parent)
      :
      QGraphicsView(parent),
      library_(nullptr),
      toolchain_(nullptr),
      zoomLevel_(1.0),
      newConnection_(nullptr)
    {
    }

    void ToolchainEditor::setToolchain(Toolchain* _toolchain)
    {
      toolchain_=_toolchain;
      buildToolchain();
    }

    bool ToolchainEditor::inNewElementMode() const
    {
      return newTool_ || newConnection_ || newItem_ || newDevice_;
    }

    void ToolchainEditor::leaveNewElementMode()
    {
      auto _delete = [&](QGraphicsItem* _item)
      {
        if (!_item) return;
        _item->prepareGeometryChange();
        scene()->removeItem(_item);
        delete _item;
      };

      _delete(newTool_);
      _delete(newConnection_);
      _delete(newItem_);
      _delete(newDevice_);
    }

    void ToolchainEditor::removeSelected()
    {

      auto _delete = [&](QGraphicsItem* _item)
      {
        if (!_item) return;
        if (!_item->isSelected()) return;
        _item->prepareGeometryChange();
        scene()->removeItem(_item);
        delete _item;
      };
      
      for (auto& _edge : toolEdges_)
      {
        if (!_edge) continue;
        scene()->removeItem(_edge);
        delete _edge;
      }

      for (auto& _tool : toolNodes_)
      {
        _delete(_tool);
      }
      eraseNullPointers(toolNodes_);
      for (auto& _item : itemNodes_)
      {
        _delete(_item);
      }
      eraseNullPointers(itemNodes_);
      for (auto& _device : deviceNodes_)
      {
        _delete(_item);
      }
      eraseNullPointers(deviceNodes_);
      rebuildPortList();
      rebuildToolEdges();
    }

    void ToolchainEditor::rebuildPortList()
    { 
      ports_.clear();
      for (auto& _item : itemNodes_)
        ports_.push_back(_item->port());

      for (auto& _tool : toolNodes_)
        for (auto& _port : _tool->ports())
        {
          ports_.push_back(_port);
        }
      
      for (auto& _device : deviceNodes_)
        for (auto& _port : _device->ports())
        {
          ports_.push_back(_port);
        }
    }
    
    void ToolchainEditor::rebuildTooledges()
    {
      auto _delete = [&](QGraphicsItem* _item)
      {
        if (!_item) return;
        _item->prepareGeometryChange();
        scene()->removeItem(_item);
        delete _item;
      };
      for (auto& _edge : toolEdges_)
      {
        _delete(_edge);
      }

      if (toolNodes_.size() < 2) return;

      for (size_t i = 0; i < toolNodes_.size()-1; ++i)
      {
        toolEdges_.emplace_back(toolNodes_[i],toolNodes_[i+1]);
        scene().addItem(toolEdges_.back());
      }

      toolNodes_.front().setEdges(nullptr,toolEdges[0]);
      for (size_t i = 1; i < toolEdges_.size(); ++i)
      {
        toolNodes_[i].setEdges(toolEdges_[i-1],toolEdges[i]);
      }
      toolNodes_.back().setEdges(nullptr,toolEdges.back());
    }


 
    void ToolchainEditor::clear()
    {
      leaveNewElementMode();
      toolEdges_.clear();
      toolNodes_.clear();
      itemNodes_.clear();
      devicesNodes_.clear();
      ports_.clear();
    }

    void Toolchain::buildToolchain()
    {
      if (!toolchain_) return;

      clear();
      for (auto& _tool : toolchain_->tools())
      {
        addTool(_tool,tools()->get(_tool));
      }
      
      for (auto& _idItem : toolchain_->items())
      {
        addItem(_idItem.first,_idItem.second);
      }
      
      for (auto& _idDevice : toolchain_->devices())
      {
        addDevice(_idDevice.first,_idDevice.second);
      }
      rebuildToolEdges();
      update();
    }

    void 


    void ToolchainEditor::addTool(const QPointF& const std::string& _name)
    {
      toolNodes_.push_back(new ToolNode(this,_name));
      auto& _tool = toolNodes_.back();
      scene()->addItem(_tool);
    }

    tool::Node* enterNewToolMode(const QPointF& _pos, const std::string& _typeId)
    {
      if (

    }





    void ToolchainEditor::update()
    {
      for (auto _node : toolNodes_)
      {
        _node->update();
      }
      QGraphicsView::update();
    }


    void ToolchainEditor::keyPressEvent(QKeyEvent *event)
    {
      switch (event->key())
      {
      case Qt::Key_Plus:
        scaleView(qreal(1.2));
        break;
      case Qt::Key_Minus:
        scaleView(1 / qreal(1.2));
        break;
      case Qt::Key_Space:
      case Qt::Key_Enter:
for (QGraphicsItem *item : scene()->items())
        {
          if (qgraphicsitem_cast<ToolNode *>(item))
            item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
        }
        break;
      default:
        QGraphicsView::keyPressEvent(event);
      }
    }


    void ToolchainEditor::scaleView(qreal scaleFactor)
    {
      zoomLevel_ = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
      if (zoomLevel_ < 0.07 || zoomLevel_ > 100)
        return;

      scale(scaleFactor, scaleFactor);
    }
  }
}

