#pragma once

#include <QGraphicsView>

namespace tomo
{
  namespace widget
  {
    class ToolchainThumbnail : 
      public QGraphicsView
    {
      Q_OBJECT
    public:
      ToolchainThumbnail(QWidget* _parent = nullptr) : 
        QGraphicsView(_parent)
      {
      }
      
      ~ToolchainThumbnail()
      {
      }
    };
  }
}
