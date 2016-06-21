#pragma once

#include "../ColorRange.h"

namespace tomo
{
  namespace widget
  {
    namespace convert
    {
      class ColorRange : 
        public Converter,
        private Registrar<ColorRange>
      {
      public:
        TBD_CONVERTER("ColorRange")
        ColorRange(std::string const& _name, const parameters_type& _value) : 
          Converter(_name,_value) {}

        inline QWidget* makeWidget(ParameterForm* _form, const parameters_type& _p) const
        {
          widget::ColorRange* _clr = new widget::ColorRange();  
          ParameterForm::connect(_clr, SIGNAL(valueChanged()), _form, SLOT(update()));
          updateWidget(_clr);  
          return _clr;
        }
        
        inline void updateWidget(QWidget* _widget) const
        {
          widget::ColorRange* _clr = static_cast<widget::ColorRange*>(_widget);           
          tomo::ColorRange3 _clrRange;
          tbd::Serialize<tomo::ColorRange3>::load(_clrRange,"",parameters());
          _clr->setValue(_clrRange);
        }

        inline void toParameter(QWidget const* _widget, parameters_type& _p) const
        {
          if (!_widget) return;
          widget::ColorRange const* _clrWidget = static_cast<widget::ColorRange const*>(_widget); 
          tbd::Serialize<tomo::ColorRange3>::save(_clrWidget->value(),name(),_p);
        }
      };
    }
  }
}
