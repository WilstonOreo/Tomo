#include "MainWindow.h"
#include <QFileDialog>
#include <QKeyEvent>
#include <QLineEdit>
#include <tomo/tool/Chain.hpp>

#include "widget/ToolchainNavigator.h"
#include "widget/ElementForm.h"

//#include "widget/VisualizerForm.h"

using namespace tomo;

MainWindow::MainWindow(
    tomo::tool::Library* _library,
    std::string const& _toolchainFile,
    QMainWindow *_parent) : 
  QMainWindow(_parent),
  library_(_library),
  toolchain_(new tomo::Toolchain())
{
  setupUi(this);
  QMainWindow::tabifyDockWidget(dockLibrary, dockNavigator);
  QMainWindow::tabifyDockWidget(dockLibrary,dockParameters);

  dockLibrary->raise();
  
  toolBar->hide();
  navigator->grpThumbnail->hide();

  QList<int> _sizes;
  _sizes << (width() / 2 - width() / 5);
  _sizes << (width() / 2 - width() / 5);

  splitter->setSizes(_sizes);

  dockLibrary->resize(QSize(width()/5,height()));
  dockParameters->resize(QSize(width()/5,height()));
  dockNavigator->resize(QSize(width()/5,height()));

  connect(
      toolchainEditor,SIGNAL(visualizersChanged()),
      this,SLOT(updateSelection()));
      
  connect(toolchainEditor->scene(),SIGNAL(selectionChanged()),
      this,SLOT(updateSelection()));

  connect(toolchainEditor,SIGNAL(toolchainChanged()),
      navigator->tree,SLOT(updateToolchain()));  
  connect(toolchainEditor,SIGNAL(toolchainChanged()),
      parameters->elementList,SLOT(updateToolchain()));
  connect(toolchainEditor,SIGNAL(toolchainChanged()),
      this,SLOT(updateSelection()));

 
  connect(
      parameters,SIGNAL(visualizerChanged()),
      this,SLOT(viewerUpdate()));



  /// Make connections with all main GUI elements
/*
  connect(toolchainEditor,SIGNAL(changed()),
      this,SLOT(editorUpdate())); 
  connect(parameters,SIGNAL(changed()),
      this,SLOT(parametersUpdate()));
  connect(toolchainTree,SIGNAL(changed()),
      this,SLOT(treeUpdate()));
*/


  // Assign tool library to library widget 
  toolLibrary->library(_library);
  toolchainEditor->setLibrary(_library);
  toolchainEditor->setToolchain(toolchain_.get());
  if (!_toolchainFile.empty())
  {
    loadToolchain(_toolchainFile);
  }
  toolchainVisualizer->update();

  navigator->setEditor(toolchainEditor);  
  parameters->setEditor(toolchainEditor);
  toggle();
}


MainWindow::~MainWindow()
{
}
  
void MainWindow::viewerUpdate()
{
  if (toolchainEditor->mode() != ToolchainEditor::READY) return;

  auto&& _elements = toolchainEditor->getSelectedElements(true);
  ToolchainEditor::visualizers_type _visualizers;
  for (auto& _vis : toolchainEditor->visualizers())
  {
    if (_elements.count(_vis->element()))
    {
      _vis->preprocess();
      _visualizers.insert(_vis.get());
    }
  }
  toolchainVisualizer->setItems(_visualizers);
  toolchainVisualizer->update();
}

void MainWindow::loadToolchain(std::string const& _filename)
{
  toolchain_->loadFromFile(_filename);
  toolchainEditor->setToolchain(toolchain_.get());

  //  toolchainEditor->update(toolchain_.get());
}

void MainWindow::loadToolchain()
{
  QString _filename = QFileDialog::getOpenFileName(this, "Open Toolchain from File...", ".", "Tomo Toolchain Files (*.tomo)");
  if( !_filename.isEmpty() )
  {
    loadToolchain(_filename.toStdString());
  }
}


void MainWindow::keyPressEvent(QKeyEvent* event)
{
  
  switch( event->key() )
  {
  case Qt::Key_F5:
      toolchainEditor->processToolchain();
      break;
  }
}
  
void MainWindow::editorUpdate()
{
  toolchainVisualizer->update();
}

void MainWindow::parametersUpdate()
{
  toolchainVisualizer->update();
}

void MainWindow::treeUpdate()
{
  toolchainVisualizer->update();
}

void MainWindow::updateSelection()
{
  if (toolchainEditor->mode() != ToolchainEditor::READY) return;

  auto&& _elements = toolchainEditor->getSelectedElements(true);

  auto&& _deviceVisualizers = toolchainEditor->getDeviceVisualizers();
  if (_deviceVisualizers.empty())
  {
    toolchainVisualizer->setDevice(nullptr);
  } else
  {
    for (auto& _vis : _deviceVisualizers)
    {
      _vis->preprocess();
      toolchainVisualizer->setDevice(_vis);
      break;
    }
  }
  toolchainVisualizer->update();
  viewerUpdate();

  _elements = toolchainEditor->getSelectedElements(false);
  if (_elements.size() == 1)
  {
    base::Element* _element = const_cast<base::Element*>(*_elements.begin());
    parameters->setElement(_element);
  } else
  {
    parameters->setElement(nullptr);
  }
  
}

void MainWindow::toggle()
{
//  glWidget->setVisible(!chkShowEditor->isChecked());
 // toolchainEditor->setVisible(chkShowEditor->isChecked());
}


