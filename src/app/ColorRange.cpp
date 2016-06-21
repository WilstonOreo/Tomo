#include "widget/ColorRange.h"

#include "qutils.hpp"

namespace tomo
{
  namespace widget
  {
    ColorRange::ColorRange(QWidget* _parent) :
      QWidget(_parent)
    {
      minColorButton_.reset(new ColorButton(this));
      maxColorButton_.reset(new ColorButton(this));
      minSpinBox_.reset(new QDoubleSpinBox(this));
      maxSpinBox_.reset(new QDoubleSpinBox(this));

      connect(minColorButton_.get(),SIGNAL(clicked()),this,SLOT(changeMinColor()));
      connect(maxColorButton_.get(),SIGNAL(clicked()),this,SLOT(changeMaxColor()));
      connect(minSpinBox_.get(),SIGNAL(valueChanged(double)),this,SLOT(changeMin()));
      connect(maxSpinBox_.get(),SIGNAL(valueChanged(double)),this,SLOT(changeMax()));

      layout_.reset(new QGridLayout());
      setLayout(layout_.get());
      layout_->addWidget(minColorButton_.get(),0,0);
      layout_->addWidget(maxColorButton_.get(),0,1);
      layout_->addWidget(minSpinBox_.get(),1,0);
      layout_->addWidget(maxSpinBox_.get(),1,1);
      setRange(gex::Range(-1000,1000),0.05);
    }
    
    void ColorRange::changeMinColor()
    {
      auto&& _color = minColorButton_->color();
      value_.minColor() = qt::color(_color);
      emit valueChanged();
    }
    void ColorRange::changeMaxColor()
    {
      auto&& _color = maxColorButton_->color();
      value_.maxColor() = qt::color(_color);
      emit valueChanged();
    }
    void ColorRange::changeMin()
    {
      value_.range().min() = minSpinBox_->value();
      emit valueChanged();
    }
    void ColorRange::changeMax()
    {
      value_.range().max() = maxSpinBox_->value();
      emit valueChanged();
    }

    void ColorRange::setRange(const gex::Range& _range, double _step)
    {
      minSpinBox_->setRange(_range.min(),_range.max());
      minSpinBox_->setSingleStep(_step);
      maxSpinBox_->setRange(_range.min(),_range.max());
      maxSpinBox_->setSingleStep(_step);
    }

    void ColorRange::setValue(const colorrange_type& _colorRange)
    {
      value_=_colorRange;
      minColorButton_->color(qt::color(_colorRange.minColor()));
      maxColorButton_->color(qt::color(_colorRange.maxColor()));
      minSpinBox_->setValue(_colorRange.range().min());
      maxSpinBox_->setValue(_colorRange.range().max());
    }
  }
}
