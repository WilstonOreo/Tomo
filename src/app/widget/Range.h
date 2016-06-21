#pragma once

#include "ui_Range.h"
#include <tomo/base/gex.hpp>

namespace tomo
{
  namespace widget
  {
    class Range : public QWidget, public Ui::Range
    {
      Q_OBJECT
    public:
      Range(QWidget* _parent =nullptr);
      ~Range();

      void setValue(tomo::Range const&);

      TBD_PROPERTY_REF_RO(tomo::Range,value)
    private slots:
      void changeMin(double);
      void changeMax(double);

    signals:
      void valueChanged();
    };
  }
}
