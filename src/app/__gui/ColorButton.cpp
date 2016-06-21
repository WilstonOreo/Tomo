#include "widget/ColorButton.h"

#include <QColorDialog>
#include <sstream>

namespace tomo
{
  ColorButton::ColorButton(QWidget* _parent) :
    QPushButton(_parent)
  {
    this->setFlat(true);

    ColorButton::connect(this, SIGNAL(clicked()), this, SLOT(click()));
  }

  ColorButton::ColorButton(const color_type& _color, QWidget* _parent) :
    QPushButton(_parent)
  {
    this->setFlat(true);
    color(_color);
  }

  void ColorButton::str(const std::string& _str)
  {
    std::stringstream ss(_str);
    color_type _color;
    ss >> _color;
    color(_color);
  }

  void ColorButton::color(const color_type& _color)
  {
    color_ = _color;
    this->setStyleSheet("* { background-color: "+colorToQString(color_)+"}");
  }

  void ColorButton::click()
  {
    this->setStyleSheet("");
    QColor _qcolor = QColorDialog::getColor(colorToQColor(color_), this);
    if (_qcolor.isValid())
    {	
      color(color_type(_qcolor.redF(),_qcolor.greenF(),_qcolor.blueF()));
    } else
    {
      color(color_);
    }
  }

  QString ColorButton::colorToQString(const color_type& _c)
  {
    std::stringstream ss;
    ss << "rgb(" << conv(_c.r()) << "," << conv(_c.g()) << "," << conv(_c.b()) << ")";
    return QString().fromStdString(ss.str());
  }

  QColor ColorButton::colorToQColor(const color_type& _c)
  {
    return QColor(conv(_c.r()),conv(_c.g()),conv(_c.b()));
  }
}
