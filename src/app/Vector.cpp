#include "widget/Vector.h"

namespace tomo
{
  namespace widget
  {
    Vector::Vector(QWidget* _parent) :
      QWidget(_parent),
      value_(0,0,0)
    {
      setupUi(this);
    }

    Vector::~Vector() {}

    void Vector::changeX(double _d)
    {
      value_.x() = _d;
      emit valueChanged();
    }
    
    void Vector::changeY(double _d)
    {
      value_.y() = _d;
      emit valueChanged();
    }

    void Vector::changeZ(double _d)
    {
      value_.z() = _d;
      emit valueChanged();
    }

    void Vector::setValue(const Vec3& _v)
    {
      value_=_v;
      boxX->setValue(value_.x());
      boxY->setValue(value_.y());
      boxZ->setValue(value_.z());
    }
  }
}
