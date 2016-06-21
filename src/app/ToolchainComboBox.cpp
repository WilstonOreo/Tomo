
#include "widget/ToolchainComboBox.h"
#include "widget/ToolchainEditor.h"

#include <QTreeView>
#include <QHeaderView>

namespace tomo
{
  namespace widget
  {
    ToolchainComboBox::ToolchainComboBox(QWidget* _parent) :
      QComboBox(_parent),
      editor_(nullptr)
    {
      tree_ = new QTreeView(this);
      setView(tree_);
      model_.reset(new ToolchainTreeModel(true,this));

      tree_->setHeaderHidden(true);
      tree_->setItemsExpandable(false);
      tree_->header()->setResizeMode(QHeaderView::ResizeToContents);
      setModel(model_.get());
    }

    ToolchainComboBox::~ToolchainComboBox() {}

    bool ToolchainComboBox::empty() const
    {
      return !editor_ || (model_->rowCount() == 0);
    }

    void ToolchainComboBox::setEditor(ToolchainEditor* _editor)
    {
      editor_=_editor;
      updateToolchain();
    }
      
    void ToolchainComboBox::updateToolchain()
    {
      if (!editor_)
      {
        model_->clear();
        return;
      }
      model_->setToolchain(editor_->toolchain());
      tree_->expandAll();
    }

    base::Element const* ToolchainComboBox::getElement() const
    {
      auto&& _selected = tree_->currentIndex();
      if (!_selected.isValid()) return nullptr;

      return static_cast<treeitem::Interface*>(model_->itemFromIndex(_selected))->element();

    }
      
    void ToolchainComboBox::setElement(base::Element const* _element)
    {
      auto&& _items = model_->findItems("",Qt::MatchRecursive | Qt::MatchContains);
      for (auto& _item : _items)
      {
        auto _i = static_cast<treeitem::Interface*>(_item);
        if (_i->element() == _element)
        {
          auto&& _index = model_->indexFromItem(_item);
          tree_->setCurrentIndex(_index);
          setCurrentIndex(_index.row());
          break;
        }
      }
    }
   
  }
}
