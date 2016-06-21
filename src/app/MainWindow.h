#pragma once

#include "ui_MainWindow.h"
#include <tbd/property.h>
#include <tomo/tool/Chain.hpp>

namespace tomo
{
  namespace tool
  {
    struct Library;
  }
}

class MainWindow : public QMainWindow, public Ui::MainWindow
{
  Q_OBJECT
public:
  MainWindow (
      tomo::tool::Library* _library,
      std::string const& _toolchainFile,
      QMainWindow *parent = nullptr);
  ~MainWindow();

  void loadToolchain(std::string const& _filename);

  TBD_PROPERTY(tomo::tool::Library*,library)
protected:
  virtual void keyPressEvent(QKeyEvent* even);


private slots:
  void loadToolchain();

  void updateSelection();

  void editorUpdate();
  void parametersUpdate();
  void treeUpdate();
  void viewerUpdate();

  void toggle();
private: 
  
  std::unique_ptr<tomo::tool::Chain> toolchain_;
};

