#pragma once

#include <QScrollArea>

namespace tomo
{
  namespace widget
  {
    class VerticalScrollArea : public QScrollArea
    {
      Q_OBJECT
    public:
      VerticalScrollArea(QWidget* _parent = nullptr);

    private:
      bool eventFilter(QObject*,QEvent*);

      QWidget* scrollAreaWidgetContents_; 
    };
  }
}
