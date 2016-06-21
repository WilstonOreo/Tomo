#pragma once

#include <tomo/base/gex.hpp>
#include "../Rotation.h"

namespace tomo
{
  namespace widget
  {
    namespace convert
    {
      class Rotation : 
        public Converter,
        private Registrar<Rotation>
      {
      public:
        TBD_CONVERTER("Rotation")
        Rotation(std::string const& _name, const parameters_type& _value) : 
          Converter(_name,_value) {}

        inline QWidget* makeWidget(ParameterForm* _form, const parameters_type& _p) const
        {
          widget::Rotation* _rot = new widget::Rotation();  
          ParameterForm::connect(_rot, SIGNAL(valueChanged()), _form, SLOT(update()));
          updateWidget(_rot);
          return _rot;
        }
        
        inline void updateWidget(QWidget* _widget) const
        {
          widget::Rotation* _rot = static_cast<widget::Rotation*>(_widget);           
          tomo::Vec3 _rotation;
          tbd::Serialize<tomo::Vec3>::load(_rotation,"",parameters());
          _rot->setValue(_rotation);
        }

        inline void toParameter(QWidget const* _widget, parameters_type& _p) const
        {
          if (!_widget) return;
          widget::Rotation const* _rot = static_cast<widget::Rotation const*>(_widget); 
          tbd::Serialize<tomo::Vec3>::save(_rot->value(),name(),_p);
        }
      };
    }
  }
}

