#include "widget/ToolchainWidget.h"

#include <tomo/visual/Mesh.hpp>

#include <QHeaderView>

namespace tomo
{
  ToolchainWidgetItem::ToolchainWidgetItem(const std::string& _text,
      base::Element* _element) : 
    QStandardItem(QString().fromStdString(_text)),
    element_(_element)
  {
  }

  ToolchainWidget::ToolchainWidget(QWidget* _parent) :
    inherited_type(_parent)
  {
    prepareModel();
  }

  bool ToolchainWidget::loadConfig(const std::string& _filename)
  {
    config_.load(_filename);
    return loadToolchain(toolchainFilename_);
  }

  bool ToolchainWidget::loadToolchain(const std::string& _filename)
  {
    toolchainFilename_ = _filename;
    prepareModel();
    toolchain_.clear();


    if (!toolchain_.loadFromFile(_filename)) return false;

    toolchain_.process();
    if (viewer()) 
    {
         viewer()->initialize(toolchain());
    }
 
    
    generate(&toolchain_,root());
    inquirer_.reset(new query::Toolchain(toolchain_,queryFilename()));
    this->setModel(model_.get());
    this->expandAll();
    this->header()->setStretchLastSection(false);
    this->header()->setResizeMode(1, QHeaderView::Stretch);
    writeQuery();
    
    viewer()->toolchainVisualizer()->preprocess();
    viewer()->toolchainVisualizer()->process();
    

    return true;
  }

  void ToolchainWidget::writeQuery()
  {
    writeQuery(queryFilename());
  }

  void ToolchainWidget::writeQuery(const std::string& _queryFilename, element_type const* _element)
  {
    if (_queryFilename.empty() || !inquirer_) return;

    inquirer_->output_filename() = _queryFilename;
    if (!_element)
    {
      inquirer_->process(); 
    }
  }
  
  ToolchainWidget::widget_item_type* ToolchainWidget::root()
  {
    return static_cast<widget_item_type*>(model_->invisibleRootItem());
  }

  void ToolchainWidget::prepareModel()
  {
    if (!model_)
      model_.reset(new QStandardItemModel());
    model_->clear();
    model_->setColumnCount(2);
    model_->setHeaderData(0,Qt::Horizontal,"Element");
    model_->setHeaderData(1,Qt::Horizontal,"Name");
  }

  ToolchainWidget::elementset_type ToolchainWidget::checkedItems()
  {
    elementset_type _checkedItems;
    for (auto& _class : item::Registry::classes())
    {
      auto&& _items = model_->findItems(QString().fromStdString(_class.first),Qt::MatchRecursive | Qt::MatchStartsWith);
      for (auto _item : _items)
        if (_item->checkState() == Qt::Checked)
          _checkedItems.insert(static_cast<widget_item_type*>(_item)->element());
    }  
    
    for (auto& _class : tool::Registry::classes())
    {
      auto&& _items = model_->findItems(QString().fromStdString(_class.first),Qt::MatchRecursive | Qt::MatchStartsWith);
      for (auto _item : _items)
        if (_item->checkState() == Qt::Checked)
          _checkedItems.insert(static_cast<widget_item_type*>(_item)->element());
    }  
    return _checkedItems;
  }

  ToolchainWidget::element_type* ToolchainWidget::selection()
  {
    if (this->selectedIndexes().empty()) return nullptr;
    auto _widgetItem = model_->itemFromIndex(this->selectedIndexes()[0]);
    auto _element = static_cast<widget_item_type*>(_widgetItem)->element();
    if (!item::Registry::exists(_element->getTypeId()) &&
        !tool::Registry::exists(_element->getTypeId())) return nullptr;

    return _element;
  }


  void ToolchainWidget::generate(const tomo::tool::ChainInterface* _toolchain,
                                widget_item_type* _root)
  {
    for (auto& _idItem : _toolchain->items())
    {
      auto _item = _idItem.second.get();
      auto&& _typeId = _item->getTypeId();

      widget_item_type* _treeItem = new widget_item_type(_typeId,_item);

      if (base::hasVisualizer(_item) && viewer())
      {
        _treeItem->setCheckable(true);
        _treeItem->setCheckState(Qt::Unchecked);
      }

      addItem(_root,std::vector<QStandardItem*>(
      {
        _treeItem,
        new widget_item_type(_idItem.first,_item),
      }));
    }

    for (auto& _toolPtr : _toolchain->tools())
    {
      tomo::tool::Interface* _tool = _toolPtr.get();
      widget_item_type *_item = new widget_item_type(_tool->getTypeId(),_tool);
      
      if (base::hasVisualizer(_tool) && viewer())
      {
        _item->setCheckable(true);
        _item->setCheckState(Qt::Unchecked);
      }

      if (_tool->hasChildren())
      {
        generate(static_cast<tomo::tool::ChainInterface const*>(_tool),_item);
      }

      addItem(_root,std::vector<QStandardItem*>({_item}));
    }
  }
}

