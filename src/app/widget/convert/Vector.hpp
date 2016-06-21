
#include "../Vector.h"

namespace tomo
{
  namespace widget
  {
    namespace convert
    {
      class Vector : 
        public Converter,
        private Registrar<Vector>
      {
      public:
        TBD_CONVERTER("Vector")
        Vector(std::string const& _name, const parameters_type& _value) : 
          Converter(_name,_value) {}

        inline QWidget* makeWidget(ParameterForm* _form, const parameters_type& _p) const
        {
          widget::Vector* _vec = new widget::Vector();  
          updateWidget(_vec);
          ParameterForm::connect(_vec, SIGNAL(valueChanged()), _form, SLOT(update()));
          return _vec;
        }
        
        inline void updateWidget(QWidget* _widget) const
        {
          widget::Vector* _vecWidget = static_cast<widget::Vector*>(_widget);           
          tomo::Vec3 _vec(0,0,0);
          tbd::Serialize<tomo::Vec3>::load(_vec,"",parameters());
          _vecWidget->setValue(_vec);
        }

        inline void toParameter(QWidget const* _widget, parameters_type& _p) const
        {
          if (!_widget) return;
          widget::Vector const* _vecWidget = static_cast<widget::Vector const*>(_widget); 
          std::cout << _vecWidget->value() << std::endl;
          tbd::Serialize<tomo::Vec3>::save(_vecWidget->value(),name(),_p);
        }
      };
    }
  }
}
