#pragma once

#include "ui_ToolchainNavigator.h"
#include <tbd/property.h>

namespace tomo
{
  namespace tool
  {
    class Chain;
  }
  class ToolchainEditor;

  class ToolchainNavigator : 
    public QWidget,
    public Ui::ToolchainNavigator
  {
    Q_OBJECT
  public:
    ToolchainNavigator(QWidget* _parent = nullptr);
    ~ToolchainNavigator();
  
    tool::Chain* toolchain();


    void setEditor(ToolchainEditor* _editor);

    TBD_PROPERTY_RO(ToolchainEditor*,editor)
  };
}
