#pragma once

#include <memory>
#include <QComboBox>
#include <QTreeView>
#include "ToolchainTreeModel.h"

namespace tomo
{
  class ToolchainEditor;
  namespace base
  {
    class Element;
  }

  namespace widget
  {
    class ToolchainComboBox : 
      public QComboBox
    {
      Q_OBJECT
    public:
      ToolchainComboBox(QWidget* _parent = nullptr);
      ~ToolchainComboBox();

      void setEditor(ToolchainEditor* _editor);

      base::Element const* getElement() const;
      void setElement(base::Element const*);

      bool empty() const;

      TBD_PROPERTY_RO(ToolchainEditor*,editor)
      TBD_PROPERTY_RO(QTreeView*,tree)
    public slots:
      void updateToolchain();
    private:
      std::unique_ptr<ToolchainTreeModel> model_; 
    };
  }
}
