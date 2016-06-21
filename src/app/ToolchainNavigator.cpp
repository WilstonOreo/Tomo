
#include "widget/ToolchainNavigator.h"
#include "widget/ToolchainEditor.h"

namespace tomo
{
  ToolchainNavigator::ToolchainNavigator(QWidget* _parent) :
    QWidget(_parent),
    editor_(nullptr)
  {
    setupUi(this);
  }
    
  ToolchainNavigator::~ToolchainNavigator()
  {
  }  
    
  void ToolchainNavigator::setEditor(ToolchainEditor* _editor)
  {
    editor_=_editor;
    //thumbnail->setEditor(_editor);
    tree->setEditor(_editor);
  }
}
