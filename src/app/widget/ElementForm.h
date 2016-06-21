#pragma once

#include "ui_ElementForm.h"

#include <tomo/base/Element.hpp>

namespace tomo
{
  class ToolchainEditor;
 
  namespace visual
  {
    class Interface;
  }

  namespace query
  {
    class Interface;
  }

  namespace widget
  {
    class ElementForm : public QWidget, public Ui::ElementForm
    {
      Q_OBJECT
    public:
      ElementForm(QWidget* _parent = nullptr);
      ~ElementForm();

      void setEditor(ToolchainEditor* _editor);
      void setElement(tomo::base::Element*);
     
      TBD_PROPERTY_RO(ToolchainEditor*,editor)
      TBD_PROPERTY_RO(base::Element*,element)  
    
    signals:
      void elementChanged();
      void visualizerChanged();

    public slots:
      void setElement();
      void changeVisualizer();
    };

  }
}
