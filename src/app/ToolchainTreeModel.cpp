#include "widget/ToolchainTreeModel.h"

#include <tomo/item/Interface.hpp>
#include <tomo/tool/Chain.hpp>
#include <tomo/tool/Interface.hpp>
#include <tomo/device/Interface.hpp>



namespace tomo
{
  namespace widget
  {
    namespace treeitem
    {
      Category::Category(std::string const& _name) : 
        Interface(_name)
      {}

      Item::Item(std::string const& _text, item::Interface const* _item) :
        Interface(_text),
        item_(_item)
      {}

      Item::Item(item::Interface const* _item) : 
        Interface(_item->elementId().str()),
        item_(_item)
      {}

      inline base::Element const* Item::element() const 
      {
        return static_cast<base::Element const*>(item_);
      }
      
      Device::Device(std::string const& _text, device::Interface const* _device) :
        Interface(_text),
        device_(_device)
      {}

      Device::Device(device::Interface const* _device) : 
        Interface(_device->elementId().str()),
        device_(_device)
      {}
      
      inline base::Element const* Device::element() const 
      {
        return static_cast<base::Element const*>(device_);
      }
      
      Tool::Tool(std::string const& _text, tool::Interface const* _tool) :
        Interface(_text),
        tool_(_tool)
      {}

      Tool::Tool(tool::Interface const* _tool) : 
        Interface(_tool->elementId().str()),
        tool_(_tool)
      {}

      inline base::Element const* Tool::element() const 
      {
        return static_cast<base::Element const*>(tool_);
      }
    }

    ToolchainTreeModel::ToolchainTreeModel(QObject* _parent) :
      QStandardItemModel(_parent),
      list_(false)
    {
    }
    
    ToolchainTreeModel::ToolchainTreeModel(
        bool _list,
        QObject* _parent) :
      QStandardItemModel(_parent),
      list_(_list)
    {
    }

    ToolchainTreeModel::~ToolchainTreeModel()
    {
    }
    
    void ToolchainTreeModel::setToolchain(tool::Chain* _toolchain)
    {
      this->clear();
      setColumnCount(2);
      setHeaderData(0,Qt::Horizontal,"Name");
      setHeaderData(1,Qt::Horizontal,"Type");
      if (!_toolchain) return;

      if (list())
      {
        generateList(_toolchain);
      } else
      {
        generateTree(root(),_toolchain);
      }
    }

    treeitem::Interface* ToolchainTreeModel::addCategory(treeitem::Interface* _root, std::string const& _str)
    {
      QList<QStandardItem*> _row;
      treeitem::Interface* _item = new treeitem::Category(_str);
      _row << _item;
      _root->appendRow(_row);
      return _item; 
    }

    treeitem::Interface* ToolchainTreeModel::root()
    {
      return static_cast<treeitem::Interface*>(invisibleRootItem());
    }

    void ToolchainTreeModel::generateList(tomo::tool::Chain* _toolchain)
    {
      auto _root = root();

      for (auto& _idItem : _toolchain->items())
      {
        auto _item = _idItem.second.get();
        auto _id = std::string("Item: ") + _idItem.first.str();
        addItem<treeitem::Item>(_root,_id,_item);
      }
      
      for (auto& _idDevice : _toolchain->devices())
      {
        auto _device = _idDevice.second.get();
        auto _id = std::string("Device: ") + _idDevice.first.str();
        addItem<treeitem::Device>(_root,_id,_device);
      }
      
      size_t _index = 0;
      for (auto& _tool : _toolchain->tools())
      {
        auto _toolId = _toolchain->getToolId(_tool.get());
        std::stringstream _ss;
        _ss << "Tool #" << _index << ": " << _toolId.str();
        auto&& _id = _ss.str();
        addItem<treeitem::Tool>(_root,_id,_tool.get());

        auto&& _sinks = _tool->sinks();
        for (auto& _sink : _sinks)
        {
          addItem<treeitem::Item>(_root,_id + " - " + _sink.first.str(),_sink.second->item());
        }
        _index++;
      }
    }

    void ToolchainTreeModel::generateTree(treeitem::Interface* _root, tomo::tool::Chain* _toolchain)
    {
      if (!_toolchain) return;
        
      auto _itemsItem = addCategory(_root,"items");
      auto _devicesItem = addCategory(_root,"devices");
      auto _toolsItem = addCategory(_root,"tools");

      for (auto& _idItem : _toolchain->items())
      {
        auto _item = _idItem.second.get();
        addItem<treeitem::Item>(_itemsItem,_idItem.first,_item);
      }
      
      for (auto& _idDevice : _toolchain->devices())
      {
        auto _device = _idDevice.second.get();
        addItem<treeitem::Device>(_devicesItem,_idDevice.first,_device);  
      }
      
      for (auto& _tool : _toolchain->tools())
      {
        treeitem::Tool* _toolItem = addItem<treeitem::Tool>(_toolsItem,_toolchain->getToolId(_tool.get()),_tool.get());

        if (_tool->hasChildren())
        {
          generateTree(_toolItem,static_cast<tomo::tool::Chain*>(_tool.get()));
        }
      }

    }

  }
}
