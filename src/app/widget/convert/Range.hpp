
#include "../Range.h"

namespace tomo
{
  namespace widget
  {
    namespace convert
    {
      class Range : 
        public Converter,
        private Registrar<Range>
      {
      public:
        TBD_CONVERTER("Range")
        Range(std::string const& _name, const parameters_type& _value) : 
          Converter(_name,_value) {}

        inline QWidget* makeWidget(ParameterForm* _form, const parameters_type& _p) const
        {
          widget::Range* _range = new widget::Range();  
          ParameterForm::connect(_range, SIGNAL(valueChanged()), _form, SLOT(update()));
          updateWidget(_range);
          return _range;
        }
        
        inline void updateWidget(QWidget* _widget) const
        {
          widget::Range* _rangeWidget = static_cast<widget::Range*>(_widget);           
          tomo::Range _range(0,0);
          tbd::Serialize<tomo::Range>::load(_range,"",parameters());
          _rangeWidget->setValue(_range);
        }

        inline void toParameter(QWidget const* _widget, parameters_type& _p) const
        {
          if (!_widget) return;
          widget::Range const* _rangeWidget = static_cast<widget::Range const*>(_widget); 
          tbd::Serialize<tomo::Range>::save(_rangeWidget->value(),name(),_p);
        }
      };
    }
  }
}
