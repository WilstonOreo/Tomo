#pragma once

#include "ui_Vector.h"
#include <tomo/base/gex.hpp>

namespace tomo
{
  namespace widget
  {
    class Vector : public QWidget, public Ui::Vector
    {
      Q_OBJECT
    public:
      Vector(QWidget* _parent =nullptr);
      ~Vector();

      void setValue(const Vec3&);

      TBD_PROPERTY_REF_RO(Vec3,value)
    private slots:
      void changeX(double);
      void changeY(double);
      void changeZ(double);

    signals:
      void valueChanged();
    };
  }
}
