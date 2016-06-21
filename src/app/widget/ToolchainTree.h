#pragma once

#include <QTreeView>
#include "ToolchainTreeModel.h"
#include <memory>

namespace tomo
{
  class ToolchainEditor;

  namespace widget
  {
    class ToolchainTree : 
      public QTreeView
    {
      Q_OBJECT
    public:
      ToolchainTree(QWidget* _parent = nullptr); 
      ~ToolchainTree();
    
      tomo::tool::Chain* toolchain();
 
      void setEditor(ToolchainEditor* _editor);
      TBD_PROPERTY_RO(ToolchainEditor*,editor)
    public slots:
      void updateToolchain();
    private:
      void prepareModel();
      
      std::unique_ptr<ToolchainTreeModel> model_; 
    };
  }
}
