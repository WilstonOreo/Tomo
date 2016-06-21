#pragma once

#include <QTreeView>
#include <string>
#include <memory>
#include <tbd/property.h>
#include <QStandardItemModel>
#include <tomo/tool/Library.hpp>


namespace tomo
{
  namespace base
  {
    struct ElementId;
  }
  namespace tool
  {
    struct Library;
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

  class ToolLibraryItem : public QStandardItem
  {
  public:
    enum ItemType { TOOL, ITEM, DEVICE, TOOLSET, SORTER, INVALID };

    ToolLibraryItem(std::string const& _id);
    ToolLibraryItem(std::string const& _id, ItemType _type);
    ToolLibraryItem(std::string const& _toolset, std::string const& _typeId, ItemType _type);
    ~ToolLibraryItem();

    inline bool hasElement() const
    {
      return tool() || device() || item();
    }

    std::unique_ptr<tool::Interface> tool() const;
    std::unique_ptr<device::Interface> device() const;
    std::unique_ptr<item::Interface> item() const;

    TBD_PROPERTY_REF_RO(base::ElementId,id)
    TBD_PROPERTY_RO(ItemType,itemType)
  };

  class ToolLibraryTree : public QTreeView
  {
    Q_OBJECT
  public:
    ToolLibraryTree(QWidget* _parent = nullptr);
    ~ToolLibraryTree();

    void generate(const std::string&);
    QStandardItem* root();
 
    ToolLibraryItem const* getSelected() const;

    TBD_PROPERTY(tool::Library*,library)
 
  protected:
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

  private:
    void generate();
    template<typename...ARGS>
    ToolLibraryItem* addItem(QStandardItem* _root, ARGS&&..._args)
    {
      QList<QStandardItem*> _row;
      ToolLibraryItem* _item = new ToolLibraryItem(_args...);
      _item->setEditable(false);
      _row << _item;
      _root->appendRow(_row);
      return _item;
    }

    QPoint dragStartPos_;
    QStandardItemModel* model_;
  };
}
