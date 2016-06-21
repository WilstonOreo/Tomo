#include "widget/ToolLibraryTree.h"
#include <QStandardItemModel>

#include <QHeaderView>
#include <QMouseEvent>
#include <QApplication>

#include <tomo/tool/Library.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <tomo/item/Interface.hpp>
#include <tomo/tool/Interface.hpp>
#include <tomo/device/Interface.hpp>

namespace tomo
{
  ToolLibraryItem::ToolLibraryItem(std::string const& _id) : 
    QStandardItem(QString().fromStdString(_id)),
    id_(_id),
    itemType_(INVALID)
  {
    if (item::Registry::exists(_id))
    {
      itemType_ = ITEM; 
    } else
    if (tool::Registry::exists(_id))
    {
      itemType_ = TOOL; 
    } else
    if (device::Registry::exists(_id))
    {
      itemType_ = DEVICE; 
    }
  }
  
  ToolLibraryItem::ToolLibraryItem(std::string const& _id, ItemType _type) : 
    QStandardItem(QString().fromStdString(_id)),
    id_(_id),
    itemType_(_type)
  {
  }
  
  ToolLibraryItem::ToolLibraryItem(
      std::string const& _toolset,
      std::string const& _typeId,
      ItemType _type) : 
    QStandardItem(QString().fromStdString(_typeId)),
    id_(_toolset,_typeId),
    itemType_(_type)
  {
  }
    
  ToolLibraryItem::~ToolLibraryItem()
  {
  }

  std::unique_ptr<tool::Interface> ToolLibraryItem::tool() const
  {
    return itemType_ == TOOL ? tool::Registry::create(id_,nullptr) : nullptr;
  }

  std::unique_ptr<item::Interface> ToolLibraryItem::item() const
  {
    return itemType_ == ITEM ? item::Registry::create(id_,nullptr) : nullptr;
  }

  std::unique_ptr<device::Interface> ToolLibraryItem::device() const
  {
    return itemType_ == DEVICE ? device::Registry::create(id_,nullptr) : nullptr;
  }
 

  ToolLibraryTree::ToolLibraryTree(QWidget* _parent) :
    QTreeView(_parent),
    library_(nullptr),
    model_(new QStandardItemModel())
  {
    setModel(model_);
  }
  
  ToolLibraryTree::~ToolLibraryTree()
  {

  }

  ToolLibraryItem const* ToolLibraryTree::getSelected() const
  {
    if (!model_) return nullptr;
    auto&& _selected = this->selectedIndexes();
    if (_selected.empty()) return nullptr;
    return static_cast<ToolLibraryItem const*>(model_->itemFromIndex(_selected[0]));
  }

  void ToolLibraryTree::mousePressEvent(QMouseEvent* _event)
  {
    if (_event->button() == Qt::LeftButton) 
      dragStartPos_ = _event->pos();

    QTreeView::mousePressEvent(_event);
  }

  void ToolLibraryTree::mouseMoveEvent(QMouseEvent* event)
  {
     if (!(event->buttons() & Qt::LeftButton))
         return;
     if ((event->pos() - dragStartPos_).manhattanLength()
          < QApplication::startDragDistance())
         return;

     auto _selected = getSelected();

     if (!_selected) return;
     if (!_selected->hasElement())
     {
       return;
     }

     QDrag *drag = new QDrag(this);
     QMimeData *mimeData = new QMimeData;
     
     mimeData->setText(QString().fromStdString(_selected->id().str()));
     drag->setMimeData(mimeData);
    drag->start();
  }

  QStandardItem* ToolLibraryTree::root()
  {
    if (!model_) return nullptr;
    return model_->invisibleRootItem();
  }
    
  void ToolLibraryTree::generate()
  {
    auto&& _toolsets = library()->toolsets();

    for (auto& _toolset : _toolsets)
    {
      auto _item = addItem(root(),_toolset,ToolLibraryItem::TOOLSET);
      
      auto&& _devices = library()->devices(_toolset);
      auto&& _tools = library()->tools(_toolset);
      auto&& _items = library()->items(_toolset); 
  
      if (!_items.empty())
      {
        auto _itemItems = addItem(_item,"items",ToolLibraryItem::SORTER); 
        for (auto& _item : _items)
        {
          addItem(_itemItems,_toolset,_item.typeId(),ToolLibraryItem::ITEM);
        }
      }
      
      if (!_tools.empty())
      {
        auto _itemTools = addItem(_item,"tools",ToolLibraryItem::SORTER); 
        for (auto& _tool : _tools)
        {
          addItem(_itemTools,_toolset,_tool.typeId(),ToolLibraryItem::TOOL);
        }
      }
      if (!_devices.empty())
      {
        auto _itemDevices = addItem(_item,"devices",ToolLibraryItem::SORTER); 
        for (auto& _device : _devices)
        {
          addItem(_itemDevices,_toolset,_device.typeId(),ToolLibraryItem::DEVICE);
        }
      }
    }
  }

  void ToolLibraryTree::generate(const std::string& _str)
  {
    if (!library()) return;

    model_->clear();
    if (_str.empty()) 
    {
      generate();
    } else
    {
      auto&& _elements = library()->all();
      auto _lower = boost::to_lower_copy(_str);
      for (auto& _element : _elements)
      {
        auto _id = _element.str();
        if (boost::to_lower_copy(_id).find(_lower) != std::string::npos)
          addItem(root(),_id);
      }
    }

    header()->hide();
    expandAll();
  }
}
