#include "widget/Range.h"

namespace tomo
{
  namespace widget
  {
    Range::Range(QWidget* _parent) :
      QWidget(_parent),
      value_(0,0)
    {
      setupUi(this);
    }

    Range::~Range() {}


    void Range::changeMin(double _v)
    {
      value_.min() = _v;
      emit valueChanged();
    }
    void Range::changeMax(double _v)
    {
      value_.max() = _v;
      emit valueChanged();
    }

    void Range::setValue(tomo::Range const& _range)
    {
      value_ = _range;
      boxMin->setValue(value_.min());
      boxMax->setValue(value_.max());
    }
  }
}
