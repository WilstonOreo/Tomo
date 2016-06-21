#pragma once

#include <tbd/qt/ColorButton.h>
#include <tomo/visual/Color.hpp>

namespace tomo
{
  namespace widget
  {
    namespace convert
    {
      class ColorField : 
        public Converter,
        public Registrar<ColorField>
      {
      public:
        typedef tbd::qt::ColorButton ColorButton;

        TBD_CONVERTER("ColorField")
        ColorField(std::string const& _name, const parameters_type& _value) : 
          Converter(_name,_value) {}   
 
        inline QWidget* makeWidget(ParameterForm* _form, const parameters_type& _p) const
        {
          ColorButton* _clr = new ColorButton(); 
          ParameterForm::connect(_clr, SIGNAL(clicked()), _form, SLOT(update()));
          updateWidget(_clr);
          return _clr;
        }
        
        inline void updateWidget(QWidget* _widget) const
        {
          ColorButton* _c = static_cast<ColorButton*>(_widget);
          Color3 _color(0,0,0);
          tbd::Serialize<Color3>::load(_color,"",parameters());
          _c->color(QColor(_color.r()*255,_color.g()*255,_color.b()*255));
        }
        
        inline void toParameter(QWidget const* _widget, parameters_type& _p) const
        {
          if (!_widget) return;
          ColorButton const* _clr = static_cast<ColorButton const*>(_widget);
          auto&& _qcolor = _clr->color();
          Color3 _color(_qcolor.red()/255.0,_qcolor.green()/255.0,_qcolor.blue()/255.0);
          tbd::Serialize<Color3>::save(_color,name(),_p);
        }
      };
    }
  }
}
