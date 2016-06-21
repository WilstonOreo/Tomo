#pragma once
#include <QtGui/QGraphicsView>
#include <tbd/property.h>
#include <set>
#include <memory>
#include <gex/base.hpp>
#include <tomo/visual/Interface.hpp>
#include <tomo/query/Interface.hpp>
#include "ToolchainItem.h"

#include "Connection.h"
#include "DummyNode.h"

namespace tomo
{

  namespace query
  {
    class Interface;
  }

  namespace tool
  {
    class Interface;
    class Chain;
    struct Connection;
    struct Library;
  }

  namespace device
  {
    class Interface;
  }
  namespace item
  {
    class Interface;
  }

  namespace widget
  {
    class ToolNode;
    class ToolEdge;
    class ItemNode;
    class DeviceNode;
    class ElementNode;
    class Connection;
    class Port;
    class SourcePort;
    class SinkPort;
  }

  class ToolchainEditor : public QGraphicsView
  {
    Q_OBJECT
  public:
    typedef visual::Interface visual_interface_type;
    typedef query::Interface query_interface_type;
    typedef std::set<visual_interface_type*> visualizers_type;
    typedef std::set<query_interface_type*> inquirers_type;
    typedef std::vector<std::unique_ptr<visual_interface_type>> visualizer_list_type;
    typedef std::vector<std::unique_ptr<query_interface_type>> inquirer_list_type;

    enum Mode { READY, NEW_CONNECTION, NEW_ITEM, NEW_TOOL, NEW_DEVICE, UPDATE };


    ToolchainEditor(QWidget *_parent = nullptr);


    void processToolchain();
    void deleteSelected();
    
    std::set<widget::ToolchainItem const*> getSelected() const; 
    std::set<base::Element const*> getSelectedElements(bool _recursive = false) const; 

    Q_SLOT void setToolchain(tool::Chain* _toolchain);
    void setLibrary(tool::Library* _lib);

    visual_interface_type* getVisualizer(base::Element const*);
    query_interface_type* getInquirer(base::Element const*);
  
    visualizers_type getDeviceVisualizers();
  
    void deleteConnection(widget::Connection*);
    
    
    /*

              bool inNewElementMode() const;
              void leaveNewElementMode();

              void itemMoved();
              void removeConnection(Port* _port);

              void removeSelected();
    */

   
    TBD_PROPERTY_REF_RO(visualizer_list_type,visualizers)
    TBD_PROPERTY_REF_RO(inquirer_list_type,inquirers)
    TBD_PROPERTY_RO(tool::Chain*,toolchain)
    TBD_PROPERTY_RO(tool::Library*,library)
    TBD_PROPERTY_REF_RO(std::vector<widget::ToolNode*>,toolNodes)

    TBD_PROPERTY_RO(Mode,mode)

  signals:
    void visualizersChanged();
    void inquirersChanged();
    void toolchainChanged();

  public slots:
    void update();
  protected:
    //    void keyPressEvent(QKeyEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);
    
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
 
    void resizeEvent(QResizeEvent* _event);
    //void paintEvent(QPaintEvent* _event);
    void keyPressEvent(QKeyEvent* event);

    void dragEnterEvent(QDragEnterEvent*);
    void dragLeaveEvent(QDragLeaveEvent*);
    void dragMoveEvent(QDragMoveEvent*);
    void dropEvent(QDropEvent*);
  private:
    void clear();
    void buildToolchain();

    bool newElementMode() const;

    widget::Port* portAtPos(QPointF const&);
    int toolIndexAtPos(QPointF const&) const;

    void deleteVisualizersAndInquirers(base::Element const*);

    void rebuildToolEdges();

    void deletePorts(widget::ElementNode*);

    void enterNewElementMode(const QPointF& _pos, const std::string& _typeId, Mode);
    void leaveNewElementMode(const QPointF& _pos);
    void leaveNewElementMode();
    
    void enterNewConnectionMode(const QPointF& _pos, widget::Port* _port);
    void leaveNewConnectionMode(const QPointF& _pos);

    void addTool(const QPointF& _pos, const base::ElementId& _typeId);
    void addItem(const QPointF& _pos, const base::ElementId& _typeId);
    void addDevice(const QPointF& _pos, const base::ElementId& _typeId);
    void addVisualizersAndInquirers(base::Element const* _element);

    void updateSceneRect();


    widget::ToolNode* addTool(const base::Id& _id, tool::Interface* _tool);
    widget::ItemNode* addItem(const base::Id& _id, item::Interface* _item);
    widget::DeviceNode* addDevice(const base::Id& _id, device::Interface* _device);
    widget::Connection* addConnection(tool::Connection const* _connection);

    std::vector<widget::ToolEdge*> toolEdges_;
    std::set<widget::Connection*> connections_;
    std::vector<widget::ItemNode*> itemNodes_;
    std::vector<widget::DeviceNode*> deviceNodes_;
    std::set<widget::Port*> sourcePorts_;
    std::set<widget::Port*> sinkPorts_;

    std::unique_ptr<widget::Connection> newConnection_;
    std::unique_ptr<widget::DummyNode> newElement_;

    template<typename F>
    void for_each_item(F f) const
    {
      for (auto& _toolNode : toolNodes_)
      {
        f(_toolNode);
      }
      for (auto& _itemNode : itemNodes_)
      {
        f(_itemNode);
      }
      for (auto& _deviceNode : deviceNodes_)
      {
        f(_deviceNode);
      }
      for (auto& _connection : connections_)
      {
        f(_connection);
      }
      for (auto& _port : sourcePorts_)
      {
        f(_port);
      }
      for (auto& _port : sinkPorts_)
      {
        f(_port);
      }
    }

    template<typename F>
    void mode(Mode _newMode, F f)
    {
      Mode _oldMode = mode_;
      mode_ = _newMode;
      f();
      mode_ = _oldMode;
    }

    /*       void rebuildPortList();
           void rebuildTooledges();
           Config* config();

           template<typename T>
           static void eraseNullPointers(T& _t)
           {
             typedef typename T::value_type value_type;
             std::remove_if(_t.begin(),_t.end(),[](const value_type& _v)
             {
               return _v == nullptr;
             });
           }

           std::vector<Port*> ports_;

           tool::Node* newTool_;
           item::Node* newItem_;
           device::Node* newDevice_;*/
  };
}

