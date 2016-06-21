#include "widget/Rotation.h"

namespace tomo
{
  namespace widget
  {
    Rotation::Rotation(QWidget* _parent) :
      QWidget(_parent),
      updateFlag_(false)
    {
      setupUi(this);
    }

    Rotation::~Rotation() {}

    void Rotation::changeValue()
    {
      emit valueChanged();
    }

    void Rotation::boxXChanged()
    {
      if (updateFlag_) return;
      do_update([&]()
      {
        value_.x() = boxX->value();
        sliderX->setValue(value_.x());
      });
      emit valueChanged();
    }
    
    void Rotation::boxYChanged()
    {
      if (updateFlag_) return;
      do_update([&]()
      {
        value_.y() = boxY->value();
        sliderY->setValue(value_.y());
      });
      emit valueChanged();
    }
    
    void Rotation::boxZChanged()
    {
      if (updateFlag_) return;
      do_update([&]()
      {
        value_.z() = boxZ->value();
        sliderZ->setValue(value_.z());
      });
      emit valueChanged();
    }
    
    void Rotation::sliderXChanged()
    {
      if (updateFlag_) return;
      do_update([&]()
      {
        value_.x() = sliderX->value();
        boxX->setValue(sliderX->value());
      });
      emit valueChanged();
    }
    
    void Rotation::sliderYChanged()
    {
      if (updateFlag_) return;
      do_update([&]()
      {
        value_.y() = sliderY->value();
        boxY->setValue(sliderY->value());
      });
      emit valueChanged();
    }
    
    void Rotation::sliderZChanged()
    {
      if (updateFlag_) return;
      do_update([&]()
      {
        value_.z() = sliderZ->value();
        boxZ->setValue(sliderZ->value());
      });
      emit valueChanged();
    }

    void Rotation::setValue(Vec3 const& _rot)
    {
      value_=_rot;
      do_update([&]()
      {
        boxX->setValue(value().x());
        boxY->setValue(value().y());
        boxZ->setValue(value().z());
        sliderX->setValue(value().x());
        sliderY->setValue(value().y());
        sliderZ->setValue(value().z());
      });
      changeValue();  
    }
  }
}
