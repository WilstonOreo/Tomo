#include "widget/ElementForm.h"

#include <tomo/base/gex.hpp>

#include "widget/ToolchainEditor.h"
#include <tbd/qt/ParameterForm.h>

#include "widget/convert.hpp"

namespace tomo
{
  namespace widget
  {
    ElementForm::ElementForm(QWidget* _parent) : 
      QWidget(_parent),
      editor_(nullptr),
      element_(nullptr)
    {
      setupUi(this);
      setElement(nullptr);
    }

    ElementForm::~ElementForm()
    {
    }
  
    void ElementForm::setEditor(ToolchainEditor* _editor)
    {
      editor_=_editor;
      elementList->setEditor(_editor);
    }

    void ElementForm::setElement(base::Element* _element)
    {
      bool _noEditorOrEmptyToolchain = elementList->empty();
      element_=_element;

      if (!element() || _noEditorOrEmptyToolchain)
      {
        parameters->hide();
        viewParameters->hide();
        information->hide();
        return;
      }
      elementList->setElement(element_);
      if (!editor()) return;
  
      TOMO_DBG(_element->elementId()) 

      if (parameters->layout())
      {
        delete parameters->layout();
      }

      using tbd::qt::ParameterForm;
      ParameterForm::typeid_delegate_map _types = 
        ParameterForm::standardTypeIdDelegates();
      _types.insert({"Color3","ColorField"});
      _types.insert({"Range","Range"});
      _types.insert({"ColorRange3","ColorRange"});
      _types.insert({"Rotation","Rotation"});
      _types.insert({"Vec3","Vector"});

      parameters->setLayout(new tbd::qt::ParameterForm(_element,_types,parameters));
      parameters->show();

      auto _vis = editor()->getVisualizer(_element);
      if (_vis)
      {
        if (viewParameters->layout())
          delete viewParameters->layout();

        viewParameters->setLayout(new tbd::qt::ParameterForm(_vis,_types,viewParameters));
        connect(viewParameters->layout(),SIGNAL(valueChanged()),this,SLOT(changeVisualizer()));
        connect(parameters->layout(),SIGNAL(valueChanged()),this,SLOT(changeVisualizer()));
        viewParameters->show();
      } else
      {
        viewParameters->hide();
      }

      auto _query = editor()->getInquirer(_element);
      if (_query)
      {
        _query->process();
        queryView->setConfig(&_query->data());
        information->show();
      } else
      {
        information->hide();
      } 
    }
      
    void ElementForm::setElement()
    {
      setElement(const_cast<base::Element*>(elementList->getElement()));
    }

    void ElementForm::changeVisualizer()
    {
      emit visualizerChanged();
    }
  }
}
