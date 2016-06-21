#pragma once

#include "ui_Rotation.h"
#include <tomo/base/gex.hpp>

namespace tomo
{
  namespace widget
  {
    class Rotation : public QWidget, public Ui::Rotation
    {
      Q_OBJECT
    public:
      Rotation(QWidget* _parent =nullptr);
      ~Rotation();

      void setValue(Vec3 const& _rot);

      TBD_PROPERTY_REF_RO(Vec3,value)
    private slots:
      void changeValue();
      void boxXChanged();
      void boxYChanged();
      void boxZChanged();
      void sliderXChanged();
      void sliderYChanged();
      void sliderZChanged();

    signals:
      void valueChanged();
    private:
      bool updateFlag_;
      template<typename F>
      void do_update(F f)
      {
        updateFlag_ = true;
        f();
        updateFlag_ = false;
      }
    };
  }
}
