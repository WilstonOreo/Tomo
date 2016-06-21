#include "widget/ToolchainEditor.h"

#include <QProgressDialog>
#include <QApplication>

#include <tbd/boolean.h>
#include <tomo/base/assert.hpp>
#include <tomo/tool/Library.hpp>
#include <tomo/tool/Chain.hpp>
#include <tomo/tool/for_each.hpp>
#include <tomo/item/Interface.hpp>
#include <tomo/device/Interface.hpp>
#include <tomo/tool/Connection.hpp>

#include "widget/ToolchainItem.h"
#include "widget/ToolNode.h"
#include "widget/ToolEdge.h"
#include "widget/ItemNode.h"
#include "widget/DeviceNode.h"
#include "widget/Connection.h"
#include "widget/Port.h"

#include "widget/paint/ToolNode.hpp"

#include "widget/DummyNode.h"

namespace tomo
{
  ToolchainEditor::ToolchainEditor(QWidget *_parent) :
    QGraphicsView(_parent),
    toolchain_(nullptr),
    library_(nullptr),
    mode_(UPDATE),
    newConnection_(new widget::Connection(nullptr,nullptr,nullptr)),
    newElement_(nullptr)
  {
    setAcceptDrops(true);
    setMouseTracking(true);

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-200, -200, 400, 400);
    setScene(scene);
    connect(this->scene(),SIGNAL(changed(const QList<QRectF>&)),this,SLOT(update()));

    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);
    setRubberBandSelectionMode(Qt::IntersectsItemShape);
    setDragMode(RubberBandDrag);
  }

  void ToolchainEditor::updateSceneRect()
  {
      auto&& _viewportRect = QRect(0, 0, viewport()->width(), viewport()->height());
      auto&& _visibleRect = QRectF(mapToScene(_viewportRect).boundingRect());

      for (auto _toolNode : toolNodes_)
      {
        _visibleRect |= _toolNode->mapToScene(_toolNode->boundingRect()).boundingRect().normalized();
      }

      for (auto _itemNode : itemNodes_)
      {
        _visibleRect |= _itemNode->mapToScene(_itemNode->boundingRect()).boundingRect().normalized();
      }

      for (auto _deviceNode : deviceNodes_)
      {
        _visibleRect |= _deviceNode->mapToScene(_deviceNode->boundingRect()).boundingRect().normalized();
      }
      /*
      for (auto _item : scene()->items())
      _visibleRect = _visibleRect.normalized();*/
      setSceneRect(_visibleRect);

      for (auto _toolNode : toolNodes_)
      {
        _toolNode->update();
      }
      for (auto _itemNode : itemNodes_)
      {
        _itemNode->update();
      }
      for (auto _deviceNode : deviceNodes_)
      {
        _deviceNode->update();
      }
  }


  void ToolchainEditor::update()
  {
    mode(UPDATE,[this]()
    {
      scene()->update();
      for (auto& _connection : connections_)
        _connection->adjust();
      QGraphicsView::update();
    });
  }

  void ToolchainEditor::mouseMoveEvent(QMouseEvent* _event)
  {
    switch (mode_)
    {
    case NEW_CONNECTION:
    {
      QPointF _p(mapToScene(_event->pos()));
      gex::Point2 _pos(_p.x(),_p.y());
      newConnection_->destPoint(_pos);
      newConnection_->sourcePoint(_pos);

      auto _port = portAtPos(_event->pos());

      if (!newConnection_->dest() && (_port != newConnection_->source()))
        newConnection_->dest(portAtPos(_event->pos()));
      else
      if (!newConnection_->source() && (_port != newConnection_->dest()))
        newConnection_->source(portAtPos(_event->pos()));

      newConnection_->adjust();
      newConnection_->update();
      break;
    }
    default:
      break;
    }

    QGraphicsView::mouseMoveEvent(_event);
  }

  void ToolchainEditor::mousePressEvent(QMouseEvent* _event)
  {
    switch (mode_)
    {
    case READY:
    {
      auto _port = portAtPos(_event->pos());
      if (_port)
        enterNewConnectionMode(mapToScene(_event->pos()),_port);
    }
    default:
      break;
    }
    QGraphicsView::mousePressEvent(_event);
  }

  void ToolchainEditor::mouseReleaseEvent(QMouseEvent* _event)
  {
    switch (mode_)
    {
    case NEW_CONNECTION:
    {
      leaveNewConnectionMode(_event->pos());
      break;
    }
    default:
      break;
    }

    QGraphicsView::mouseReleaseEvent(_event);
    updateSceneRect();
  }

  void ToolchainEditor::enterNewConnectionMode(const QPointF& _pos, widget::Port* _port)
  {
    if (mode_ != READY || !_port)
    {
      return;
    }
    using namespace widget;


    mode_ = NEW_CONNECTION;
//    if (_port->connection())
  //    deleteConnection(_port->connection());

    auto _source = _port->portType() == Port::SOURCE ? _port : nullptr;
    auto _sink = _port->portType() == Port::SINK ? _port : nullptr;
    newConnection_->source(_source);
    newConnection_->dest(_sink);
    TOMO_DBG(_source << " " << _sink)
    this->scene()->addItem(newConnection_.get());
    newConnection_->show();
    newConnection_->setZValue(1000.0);

    newConnection_->sourcePoint() = gex::qt::adapt(_pos);
    newConnection_->destPoint() = gex::qt::adapt(_pos);
    newConnection_->adjust();

    auto&& _ports = _source ? sinkPorts_ : sourcePorts_;

    for (auto _destPort : _ports)
    {
      _destPort->highlighted(false);
      _destPort->disabled(!_destPort->connection());
      _destPort->update();

      if (_port->connection())
      {
        continue;
      }

      if (_port != _destPort)
      {
        if (_port->parentNode() == _destPort->parentNode())
        {
          _destPort->disabled(true);
          _destPort->update();
          continue;
        }
        using tbd::boolean::operator&;
        auto&& _types = _destPort->port()->supportedTypes() & _port->port()->supportedTypes();
        if (_types.empty()) continue;
      }
      _destPort->disabled(false);
      _destPort->highlighted(true);
      _destPort->update();
    }
  }

  void ToolchainEditor::leaveNewConnectionMode(QPointF const& _pos)
  {
    using namespace widget;
    auto _port = portAtPos(_pos);

    if (newConnection_->isVisible() && _port)
    {
      if (_port->portType() == Port::SINK)
        newConnection_->dest(_port);
      if (_port->portType() == Port::SOURCE)
        newConnection_->source(_port);

      auto _source = newConnection_->source();
      auto _sink = newConnection_->dest();

      if (_source && _sink)
      {
        if ((_source->portType() == Port::SINK) &&
            (_sink->portType() == Port::SOURCE))
          std::swap(_source,_sink);
        if ((_source->portType() != Port::SOURCE) &&
            (_sink->portType() != Port::SINK))
          return;
        TOMO_DBG("Connection!")

        auto&& _connId = newConnection_->connectionId();
        auto _conn = toolchain_->connect(_connId);
        if (_conn)
          addConnection(_conn);
      }
    }

    newConnection_->hide();
    this->scene()->removeItem(newConnection_.get());
    using tbd::boolean::operator|;
    auto&& _ports = sourcePorts_ | sinkPorts_;
    for (auto _port : _ports)
    {
      _port->disabled(false);
      _port->highlighted(false);
      _port->update();
    }
    mode_ = READY;
  }

  void ToolchainEditor::leaveNewElementMode()
  {
    newElement_->hide();
    this->scene()->removeItem(newElement_.get());
    using tbd::boolean::operator|;
    auto&& _ports = sourcePorts_ | sinkPorts_;
    for (auto _port : _ports)
    {
      _port->disabled(false);
      _port->highlighted(false);
      _port->update();
    }
    mode_ = READY;
  }

  void ToolchainEditor::enterNewElementMode(
    const QPointF& _pos,
    const std::string& _typeId,
    Mode _mode)
  {
    mode_ = _mode;
    if (!newElementMode()) return;

    scene()->removeItem(newElement_.get());
    newElement_->setup(_typeId,_pos);
    scene()->addItem(newElement_.get());
    newElement_->update();
  }

  void ToolchainEditor::leaveNewElementMode(const QPointF& _pos)
  {
    newElement_->hide();

    if (mode_ == NEW_ITEM)
    {
      addItem(_pos,newElement_->elementId());
    }
    else if (mode_ == NEW_TOOL)
    {
      addTool(_pos,newElement_->elementId());
    }
    if (mode_ == NEW_DEVICE)
    {
      addDevice(_pos,newElement_->elementId());
    }
    mode_ = READY;
  }

  void ToolchainEditor::resizeEvent(QResizeEvent* _event)
  {
    updateSceneRect();
    update();
    QGraphicsView::resizeEvent(_event);
  }

  std::set<widget::ToolchainItem const*> ToolchainEditor::getSelected() const
  {
    std::set<widget::ToolchainItem const*> _items;
    for_each_item([&](widget::ToolchainItem const* _item)
    {
      if (_item->isSelected()) _items.insert(_item);
    });
    return _items;
  }

  std::set<base::Element const*> ToolchainEditor::getSelectedElements(bool _recursive) const
  {
    auto&& _items = getSelected();
    std::set<base::Element const*> _elements;
    for (auto& _item : _items)
    {
      using tbd::boolean::operator|=;
      _elements |= _item->elements(_recursive);
    }
    return _elements;
  }

  void ToolchainEditor::setToolchain(tool::Chain* _toolchain)
  {
    toolchain_=_toolchain;
    processToolchain();
    buildToolchain();
  }

  void ToolchainEditor::dragEnterEvent(QDragEnterEvent *event)
  {
    auto&& _id = event->mimeData()->text().toStdString();
    Mode _newMode = UPDATE;
    if (item::Registry::exists(_id))
    {
      TOMO_DBG(_newMode);
      _newMode = NEW_ITEM;
    }
    else if (tool::Registry::exists(_id))
    {
      _newMode = NEW_TOOL;
    }
    else if (device::Registry::exists(_id))
    {
      _newMode = NEW_DEVICE;
    }
    enterNewElementMode(mapToScene(event->pos()),_id,_newMode);
    event->accept();
  }

  void ToolchainEditor::dragLeaveEvent(QDragLeaveEvent *event)
  {
    if (newElementMode())
    {
      leaveNewElementMode();
    }
    event->accept();
  }

  bool ToolchainEditor::newElementMode() const
  {
    return (mode_ == NEW_ITEM) ||
           (mode_ == NEW_TOOL) ||
           (mode_ == NEW_DEVICE);
  }

  void ToolchainEditor::dragMoveEvent(QDragMoveEvent *event)
  {
    if (!newElementMode())
    {
      event->ignore();
      return;
    }

    newElement_->setPos(mapToScene(event->pos()));
    newElement_->update();
    event->accept();
  }

  void ToolchainEditor::dropEvent(QDropEvent *event)
  {
    event->accept();
    leaveNewElementMode(mapToScene(event->pos()));
  }

  void ToolchainEditor::deleteVisualizersAndInquirers(base::Element const* _element)
  {
    if (!_element) return;
    auto&& _visNewEnd = std::remove_if(visualizers_.begin(),visualizers_.end(),
                                       [&](const std::unique_ptr<visual_interface_type>& _vis)
    {
      return _vis->element() == _element;
    });
    visualizers_.erase(_visNewEnd,visualizers_.end());

    auto&& _inqNewEnd = std::remove_if(inquirers_.begin(),inquirers_.end(),
                                       [&](const std::unique_ptr<query_interface_type>& _query)
    {
      return _query->element() == _element;
    });
    inquirers_.erase(_inqNewEnd,inquirers_.end());
  }

  void ToolchainEditor::deletePorts(widget::ElementNode* _node)
  {
    if (!_node) return;
    for (auto _port : _node->sourcePorts())
    {
      sourcePorts_.erase(_port);
    }
    for (auto _port : _node->sinkPorts())
    {
      deleteVisualizersAndInquirers(_port->port()->item());
      sinkPorts_.erase(_port);
    }
  }
  void ToolchainEditor::deleteConnection(widget::Connection* _conn)
  {
    if (!_conn) return;
    if (!_conn->valid()) return;
    auto&& _item = _conn->source()->port()->item();
    // TODO: const_cast is ugly, check const correctness in tool::Chain...
    toolchain_->disconnect(const_cast<item::Interface*>(_item));

    deleteVisualizersAndInquirers(_item);
    scene()->removeItem(_conn);
    connections_.erase(_conn);
    delete _conn;
  }

  void ToolchainEditor::deleteSelected()
  {
    mode_ = UPDATE;
    bool _deletedSomething = false;

    /// Delete selected tools
    for (auto it = toolNodes_.begin(); it != toolNodes_.end();)
    {
      auto _toolNode = *it;
      if (_toolNode->isSelected())
      {
        _deletedSomething = true;
        deleteVisualizersAndInquirers(_toolNode->tool());
        _toolNode->disconnect();

        deletePorts(_toolNode);
        TOMO_ASSERT(toolchain_->removeTool(_toolNode->tool()));
        scene()->removeItem(_toolNode);
        delete _toolNode;
        it = toolNodes_.erase(it);
      }
      else
      {
        ++it;
      }
    }

    if (_deletedSomething)
      rebuildToolEdges();

    /// Delete selected items
    for (auto it = itemNodes_.begin(); it != itemNodes_.end();)
    {
      auto _itemNode = *it;
      if (_itemNode->isSelected())
      {
        _deletedSomething = true;
        deleteVisualizersAndInquirers(_itemNode->item());
        _itemNode->disconnect();
        deletePorts(_itemNode);

        TOMO_ASSERT(toolchain_->removeItem(_itemNode->item()));
        scene()->removeItem(_itemNode);
        it = itemNodes_.erase(it);
        delete _itemNode;
      }
      else
      {
        ++it;
      }
    }

    /// Delete selected devices
    for (auto it = deviceNodes_.begin(); it != deviceNodes_.end();)
    {
      auto _deviceNode = *it;
      if (_deviceNode->isSelected())
      {
        _deletedSomething = true;
        deleteVisualizersAndInquirers(_deviceNode->device());

        _deviceNode->disconnect();
        deletePorts(_deviceNode);

        toolchain_->removeDevice(_deviceNode->device());
        scene()->removeItem(_deviceNode);
        it = deviceNodes_.erase(it);
        delete _deviceNode;
      }
      else
      {
        ++it;
      }
    }

    /// Delete selected connections
    for (auto it = connections_.begin(); it != connections_.end();)
    {
      auto _connection = *it;
      if (_connection->isSelected())
      {
        _deletedSomething = true;
        auto* _source = const_cast<tomo::item::SourceInterface*>(_connection->connection()->source());
        toolchain_->disconnect(_source);
        scene()->removeItem(_connection);
        delete _connection;
        it = connections_.erase(it);
      }
      else
      {
        ++it;
      }
    }
    if (_deletedSomething) processToolchain();
    update();
    mode_ = READY;
    emit toolchainChanged();
  }

  void ToolchainEditor::keyPressEvent(QKeyEvent* event)
  {
    switch( event->key() )
    {
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
      deleteSelected();
      break;
    }
  }

  void ToolchainEditor::rebuildToolEdges()
  {
    for (auto _toolEdge : toolEdges_)
    {
      scene()->removeItem(_toolEdge);
    }
    toolEdges_.clear();

    if (toolNodes_.empty()) return;

    toolNodes_.front()->prev(nullptr);
    toolNodes_.front()->prevTool(nullptr);
    toolNodes_.back()->next(nullptr);
    toolNodes_.back()->nextTool(nullptr);

    for (size_t i = 0; i < toolNodes_.size()-1; ++i)
    {
      auto& _prevTool = toolNodes_[i];
      auto& _nextTool = toolNodes_[i+1];
      widget::ToolEdge* _edge = new widget::ToolEdge(_prevTool,_nextTool);
      _prevTool->next(_edge);
      _prevTool->nextTool(_nextTool);
      _nextTool->prevTool(_prevTool);
      _nextTool->prev(_edge);
      toolEdges_.push_back(_edge);
      scene()->addItem(_edge);
    }
  }

  void ToolchainEditor::processToolchain()
  {
    if (!toolchain_) return;

    mode_ = UPDATE;

    QProgressDialog _progress("Processing toolchain", "Cancel", 0, 100, this);
    _progress.setWindowModality(Qt::ApplicationModal);
    _progress.setValue(0);
    _progress.raise();
    _progress.show();
    _progress.activateWindow();
    QCoreApplication::processEvents();

    size_t _idx = 0;
    size_t _numTools = toolchain_->tools().size();

    for (auto& _tool : toolchain_->tools())
    {
      if (!_tool->valid()) continue;
      std::stringstream _ss;
      _ss << toolchain_->getToolId(_tool.get()) << " (";
      _ss << _tool->elementId() << "): (";
      _ss << (_idx+1) << "/" << _numTools << ")";

      _progress.setLabelText(QString().fromStdString(_ss.str()));
      _progress.setValue(_idx * 100 / _numTools);
      _progress.update();
      QApplication::processEvents();
      _tool->process();
      _progress.update();
      QApplication::processEvents();
      ++_idx;
    }

    _progress.setLabelText(QString("Initialize visualizers..."));
    for (auto& _vis : visualizers_)
    {
      _vis->initialize();
      _progress.update();
      QApplication::processEvents();
    }
    emit this->visualizersChanged();

    _progress.setLabelText(QString("Initialize inquirers..."));

    for (auto& _inq : inquirers_)
    {
      _inq->preprocess();
      _progress.update();
      QApplication::processEvents();
    }
    emit this->inquirersChanged();

    for (auto _toolNode : toolNodes_)
      _toolNode->update();
    for (auto _deviceNode : deviceNodes_)
      _deviceNode->update();

    mode_ = READY;
  }


  widget::Port* ToolchainEditor::portAtPos(QPointF const& _point)
  {
    using tbd::boolean::operator|;
    auto&& _ports = sourcePorts_ | sinkPorts_;
    QList<QGraphicsItem*>&& _items = items(QPoint(_point.x(),_point.y()));

    std::set<widget::Port*> _portsAtPos;
    for (auto& _item: _items)
    {
      auto _port = static_cast<widget::Port*>(_item);
      std::set<widget::Port*> _p( {_port});
      using tbd::boolean::operator&;
      using tbd::boolean::operator|=;
      _portsAtPos |= _p & _ports;
    }
    if (_portsAtPos.empty()) return nullptr;

    auto _port = *_portsAtPos.begin();

    return _port->disabled() ? nullptr : _port;
    *_portsAtPos.begin();
  }

  int ToolchainEditor::toolIndexAtPos(QPointF const& _point) const
  {
    /*
    QList<QGraphicsItem*>&& _items = items(QPoint(_point.x(),_point.y()));

    using tbd::boolean::operator&;
    using tbd::boolean::operator|=;
    std::set<widget::ToolEdge*> _edgesAtPos;
    for (auto& _item: _items)
    {
    }*/
    return 0;
  }


  ToolchainEditor::visualizers_type ToolchainEditor::getDeviceVisualizers()
  {
    visualizers_type _visualizers;
    for (auto& _vis : visualizers_)
      if (device::Registry::exists(_vis->element()->elementId()))
        _visualizers.insert(_vis.get());
    return _visualizers;
  }

  void ToolchainEditor::setLibrary(tool::Library* _library)
  {
    library_=_library;

    if (newElement_)
      scene()->removeItem(newElement_.get());

    newElement_.reset(new widget::DummyNode(_library));
  }

  void ToolchainEditor::clear()
  {
    mode_ = UPDATE;
    scene()->clear();
    toolNodes_.clear();
    toolEdges_.clear();
    connections_.clear();
    itemNodes_.clear();
    deviceNodes_.clear();
    sourcePorts_.clear();
    sinkPorts_.clear();
    inquirers_.clear();
    visualizers_.clear();
    emit toolchainChanged();
    mode_ = READY;
  }

//////////////////////// Add Tools, Items, Devices, Connections

  void ToolchainEditor::addVisualizersAndInquirers(base::Element const* _element)
  {
      if (visual::has_visualizer(_element))
      {
        auto&& _vis = visual::Registry::create(_element->elementId(),_element);
        _vis->preprocess();
        visualizers_.push_back(std::move(_vis));
      }

      if (query::has_inquirer(_element))
      {
        auto&& _inq = query::Registry::create(_element->elementId(),_element);
        _inq->preprocess();
        inquirers_.push_back(std::move(_inq));
      }
  }

  widget::ToolNode* ToolchainEditor::addTool(const base::Id& _id, tool::Interface* _tool)
  {
    widget::ToolNode* _toolNode = nullptr;
    mode(UPDATE,[&]()
    {
      toolNodes_.emplace_back(new widget::ToolNode(this,_id,_tool));
      _toolNode = toolNodes_.back();
      scene()->addItem(_toolNode);
      _toolNode->setPos(100,toolNodes_.size() * widget::paint::ToolNode::radius()*1.5);
      if (toolNodes_.size() > 1)
      {
        auto _prevTool = toolNodes_[toolNodes_.size()-2];
        widget::ToolEdge* _edge = new widget::ToolEdge(_prevTool,_toolNode);
        _prevTool->next(_edge);
        _prevTool->nextTool(_toolNode);
        _toolNode->prevTool(_prevTool);
        _toolNode->prev(_edge);
        toolEdges_.push_back(_edge);
        scene()->addItem(_edge);
      }

      addVisualizersAndInquirers(_tool);

      auto&& _sourcePorts = _toolNode->sourcePorts();
      for (auto& _port : _sourcePorts)
      {

        sourcePorts_.insert(_port);
      }
      auto&& _sinkPorts = _toolNode->sinkPorts();
      for (auto& _port : _sinkPorts)
      {
        if (_port->port()->item())
          addVisualizersAndInquirers(_port->port()->item());
        sinkPorts_.insert(_port);
      }
      _toolNode->update();


    });
    mode_ = READY;
    return _toolNode;
  }

  void ToolchainEditor::addTool(const QPointF& _pos, const base::ElementId& _typeId)
  {
    mode_ = UPDATE;
    auto _tool = toolchain_->appendTool(_typeId);

    TOMO_ASSERT(_tool);

    auto&& _id = toolchain_->getToolId(_tool);
    auto _toolNode = addTool(_id,_tool);
    mode_ = UPDATE;
    _toolNode->setPos(_pos);
    _toolNode->update();
    mode_ = READY;
    emit toolchainChanged();
    emit visualizersChanged();
  }

  widget::ItemNode* ToolchainEditor::addItem(const base::Id& _id, item::Interface* _item)
  {
    mode_ = UPDATE;
    itemNodes_.emplace_back(new widget::ItemNode(this,_id,_item));
    auto* _itemNode = itemNodes_.back();
    scene()->addItem(_itemNode);

    addVisualizersAndInquirers(_item);

    auto&& _sinkPorts = _itemNode->sinkPorts();
    for (auto& _port : _sinkPorts)
    {
      sinkPorts_.insert(_port);
    }
    _itemNode->update();
    mode_ = READY;
    return _itemNode;
  }

  void ToolchainEditor::addItem(const QPointF& _pos, const base::ElementId& _typeId)
  {
    mode_ = UPDATE;

    auto _item = toolchain_->addItem(_typeId);


    TOMO_ASSERT(_item);
    auto&& _id = toolchain_->items().get(_item);
    auto _itemNode = addItem(_id,_item);
    mode_ = UPDATE;
    _itemNode->setPos(_pos);
    _itemNode->update();
    _itemNode->openFile();
    update();
    emit toolchainChanged();
    mode_ = READY;
    emit visualizersChanged();
  }

  widget::DeviceNode* ToolchainEditor::addDevice(const base::Id& _id, device::Interface* _device)
  {
    mode_ = UPDATE;
    deviceNodes_.emplace_back(new widget::DeviceNode(this,_id,_device));
    auto* _deviceNode = deviceNodes_.back();
    scene()->addItem(_deviceNode);

    addVisualizersAndInquirers(_device);

    auto&& _sourcePorts = _deviceNode->sourcePorts();
    for (auto& _port : _sourcePorts)
    {
      sourcePorts_.insert(_port);
    }

    _deviceNode->update();
    mode_ = READY;
    return _deviceNode;
  }

  void ToolchainEditor::addDevice(const QPointF& _pos, const base::ElementId& _typeId)
  {
    mode_ = UPDATE;
    auto _device = toolchain_->addDevice(_typeId);
    TOMO_ASSERT(_device);
    auto&& _id = toolchain_->devices().get(_device);
    auto _deviceNode = addDevice(_id,_device);
    mode_ = UPDATE;
    _deviceNode->setPos(_pos);
    _deviceNode->update();
    emit toolchainChanged();
    mode_ = READY;
    emit visualizersChanged();
  }

  ToolchainEditor::visual_interface_type* ToolchainEditor::getVisualizer(base::Element const* _element)
  {
for (auto& _vis : visualizers_)
      if (_vis->element() == _element)
        return _vis.get();

    return nullptr;
  }

  ToolchainEditor::query_interface_type* ToolchainEditor::getInquirer(base::Element const* _element)
  {
for (auto& _inq : inquirers_)
      if (_inq->element() == _element)
        return _inq.get();

    return nullptr;
  }

  widget::Connection* ToolchainEditor::addConnection(
    tool::Connection const* _connection)
  {
    mode_ = UPDATE;
    widget::Port* _from = nullptr;
    widget::Port* _to = nullptr;

    using tbd::boolean::operator|;
    auto&& _ports = sourcePorts_ | sinkPorts_;

    for (auto& _port : _ports)
    {
      if (_port->port() == _connection->source())
      {
        _from = _port;
        break;
      }
    }

    for (auto& _port : _ports)
    {
      if (_port->port() == _connection->sink())
      {
        _to = _port;
        break;
      }
    }
    if (!_from && !_to) return nullptr;

    auto&& _connectionIt = connections_.insert(new widget::Connection(
                             _from,
                             _to,
                             _connection));
    auto& _connectionWidget = *_connectionIt.first;
    scene()->addItem(_connectionWidget);
    _connectionWidget->update();
    mode_ = READY;
    return _connectionWidget;
  }

///////////////////////// END Add Tools, Items, Devices, Connections

  void ToolchainEditor::buildToolchain()
  {
    mode_ = UPDATE;
    clear();
    for (auto& _tool : toolchain_->tools())
    {
      addTool(toolchain_->getToolId(_tool.get()),_tool.get());
    }

    for (auto& _item : toolchain_->items())
    {
      addItem(_item.first,_item.second.get());
    }

    for (auto& _device : toolchain_->devices())
    {
      addDevice(_device.first,_device.second.get());
    }

    auto&& _connections = toolchain_->fetchConnections();
    for (auto& _connection : _connections)
      addConnection(_connection);

    update();
    updateSceneRect();
    mode_ = READY;
    emit visualizersChanged();
  }


  void ToolchainEditor::drawBackground(QPainter *painter, const QRectF &rect)
  {
    // Fill
    //auto&& _sceneRect = sceneRect();
    auto&& _sceneRect = rect.normalized();
    QLinearGradient gradient(_sceneRect.topLeft(), _sceneRect.bottomRight());
    gradient.setColorAt(0, QColor("#444444"));
    gradient.setColorAt(1, QColor("#3A3939"));
    painter->fillRect(rect, gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(rect);

    painter->setPen(QColor(0,0,0,32));

    for (int x = _sceneRect.left(); x < _sceneRect.right(); x += 32)
    {
      painter->drawLine(x,_sceneRect.top(),x,_sceneRect.bottom());
    }

    for (int y = _sceneRect.top(); y < _sceneRect.bottom(); y += 32)
    {
      painter->drawLine(_sceneRect.left(),y,_sceneRect.right(),y);
    }
  }
}
