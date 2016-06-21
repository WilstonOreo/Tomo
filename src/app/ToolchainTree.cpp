#include "widget/ToolchainTree.h"
#include "widget/ToolchainEditor.h"

#include <QHeaderView>
#include <tomo/tool/Chain.hpp>

namespace tomo
{
  namespace widget
  {
    ToolchainTree::ToolchainTree(QWidget* _parent) :
      QTreeView(_parent),
      editor_(nullptr),
      model_(nullptr)
    {
      prepareModel();
    }

    ToolchainTree::~ToolchainTree()
    {
    }


    tomo::tool::Chain* ToolchainTree::toolchain()
    {
      if (!editor_) return nullptr;

      return editor_->toolchain();
    }
      
    void ToolchainTree::setEditor(ToolchainEditor* _editor)
    {
      editor_=_editor;
      updateToolchain();
    }

    void ToolchainTree::updateToolchain()
    {
      model_->setToolchain(toolchain());
      expandToDepth(1);
    }

    void ToolchainTree::prepareModel()
    {
      if (!model_)
        model_.reset(new ToolchainTreeModel(this));

      this->setModel(model_.get());
      this->header()->setStretchLastSection(false);
      this->header()->setResizeMode(1, QHeaderView::Stretch);
      this->header()->setCascadingSectionResizes(true);
      this->header()->setResizeMode(QHeaderView::ResizeToContents);
      model_->clear();
      model_->setToolchain(toolchain()); 
    }


  }
}
