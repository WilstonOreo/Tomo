#pragma once

#include <tbd/qt/ColorButton.h>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <tomo/visual/ColorRange.hpp>

namespace tomo
{
  namespace widget
  {
    class ColorRange : public QWidget
    {
      Q_OBJECT
    public:
      typedef ColorRange3 colorrange_type;
      typedef tbd::qt::ColorButton ColorButton;

      ColorRange(QWidget* _parent = nullptr);

      void setValue(const colorrange_type&);
      void setRange(const gex::Range& _range, double _step = 0.1);

      TBD_PROPERTY_REF_RO(colorrange_type,value)
      TBD_PROPERTY_REF_RO(std::unique_ptr<ColorButton>,minColorButton)
      TBD_PROPERTY_REF_RO(std::unique_ptr<ColorButton>,maxColorButton)
      TBD_PROPERTY_REF_RO(std::unique_ptr<QDoubleSpinBox>,minSpinBox)
      TBD_PROPERTY_REF_RO(std::unique_ptr<QDoubleSpinBox>,maxSpinBox)

    private slots:
      void changeMinColor(); 
      void changeMaxColor();
      void changeMin();
      void changeMax();
    signals:
      void valueChanged();
    private:
      std::unique_ptr<QGridLayout> layout_;
    };
  }
}
