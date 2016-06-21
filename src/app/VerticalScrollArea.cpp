#include "widget/VerticalScrollArea.h"

#include <QVBoxLayout>
#include <QScrollBar>
#include <QEvent>

namespace tomo
{
  namespace widget
  {
    VerticalScrollArea::VerticalScrollArea(QWidget* _parent) :
      QScrollArea(_parent)
    {
      setWidgetResizable(true);
      setFrameStyle(QFrame::NoFrame);
      setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      scrollAreaWidgetContents_ = new QWidget(this);
      scrollAreaWidgetContents_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
      QVBoxLayout *baseLayout = new QVBoxLayout(scrollAreaWidgetContents_);
     setWidget(scrollAreaWidgetContents_);
      scrollAreaWidgetContents_->installEventFilter(this);
    };
      
    bool VerticalScrollArea::eventFilter(QObject* o,QEvent* e)
    {
      if(o == scrollAreaWidgetContents_ && e->type() == QEvent::Resize)
        setMinimumWidth(scrollAreaWidgetContents_->minimumSizeHint().width() + verticalScrollBar()->width());
      return false;
    }
  }
}
