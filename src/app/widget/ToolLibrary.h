#pragma once

#include <tbd/property.h>
#include "ui_ToolLibrary.h"

namespace tomo
{
  namespace tool
  {
    struct Library;
  }

  class ToolLibrary : public QWidget, public Ui::ToolLibrary
  {
  Q_OBJECT
  public:
    ToolLibrary(QWidget* _parent = nullptr);
    ~ToolLibrary();

    void library(tool::Library* _library);

    TBD_PROPERTY_RO(tool::Library*,library)
  public slots:
    void updateItems();
    void selectionChanged();
  private:
    QString getDescription(
        std::string const& _toolset, 
        std::string const& _typeId = std::string()) const;
  };
}
