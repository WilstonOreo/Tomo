#pragma once

#include <QStandardItemModel>
#include <tbd/property.h>

namespace tomo
{
  namespace base
  {
    class Element;
  }

  namespace tool
  {
    class Chain;
    class Interface;
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
    namespace treeitem
    {
      class Interface : public QStandardItem
      {
      public:
        Interface(std::string const& _text) : 
          QStandardItem(QString().fromStdString(_text))
        {
          setEditable(false);
        }

        enum ItemType { TOOL, ITEM, DEVICE, CATEGORY, INVALID };

        inline virtual tool::Interface const* tool() const { return nullptr; }
        inline virtual device::Interface const* device() const { return nullptr; }
        inline virtual item::Interface const* item() const { return nullptr; }
        inline virtual base::Element const* element() const { return nullptr; }
      
        virtual ItemType itemType() const = 0;
      };

      class Category : public Interface
      {
      public:
        Category(std::string const& _name);

        inline ItemType itemType() const { return CATEGORY; }
      };

      class Item : public Interface
      {
      public:
        Item(std::string const& _text, item::Interface const* _item);
        Item(item::Interface const* _item);

        inline ItemType itemType() const { return ITEM; }
        base::Element const* element() const; 

        TBD_PROPERTY(item::Interface const*,item)
      };
      
      class Device : public Interface
      {
      public:
        Device(std::string const& _text, device::Interface const* _item);
        Device(device::Interface const* _device);
        
        inline ItemType itemType() const { return DEVICE; }
        base::Element const* element() const;

        TBD_PROPERTY(device::Interface const*,device)
      };
      
      class Tool : public Interface
      {
      public:
        Tool(std::string const& _text, tool::Interface const* _item);
        Tool(tool::Interface const* _tool);
        inline ItemType itemType() const { return TOOL; }
        base::Element const* element() const;
        TBD_PROPERTY(tool::Interface const*,tool)
      };
    }

    class ToolchainTreeModel : 
      public QStandardItemModel
    {
      Q_OBJECT
    public:
      ToolchainTreeModel(QObject* _parent = nullptr); 
      ToolchainTreeModel(bool _list, QObject* _parent = nullptr);
      ~ToolchainTreeModel();
      
      treeitem::Interface* root();
      void setToolchain(tool::Chain* _toolchain);
      
      TBD_PROPERTY(bool,list) 
    private:
      void generateList(tomo::tool::Chain* _toolchain);
      void generateTree(treeitem::Interface* _root, tomo::tool::Chain* _toolchain);
     
      treeitem::Interface* addCategory(treeitem::Interface* _root, 
          std::string const& _str);
      
      template<typename ITEM_TYPE, typename ELEMENT>
      ITEM_TYPE* addItem(treeitem::Interface* _root, 
          std::string const& _name,
          ELEMENT* _element)
      {
        QList<QStandardItem*> _row;
        ITEM_TYPE* _item = new ITEM_TYPE(_name,_element);
        _row << _item;

        ITEM_TYPE* _typeIdItem = new ITEM_TYPE(_element);
        _row << _typeIdItem;
        _root->appendRow(_row);
        return _item; 
      }
    };
  }
}
