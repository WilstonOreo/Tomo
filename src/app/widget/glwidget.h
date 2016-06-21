#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL/QGLWidget>

namespace tomo
{
  class GLWidget : public QGLWidget
  {
    Q_OBJECT
  public:
//    TOMO_BASE_TYPES
    explicit GLWidget(QWidget *_parent = nullptr) : 
      QGLWidget(_parent) {}
    ~GLWidget() {}
/*
    typedef gex::Point3 Point3;
    typedef tool::Interface tool_type;
    typedef base::Element const* element_ptr_type;
    typedef visual::Interface visualizer_type;

    void initialize(Toolchain const& _toolchain);
    void clear();

    visualizer_type* visualizer(element_ptr_type _element);

    TBD_PROPERTY_REF(idset_type,visible)
    TBD_PROPERTY_REF_RO(std::unique_ptr<visual::Toolchain>,toolchainVisualizer)
  protected:
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent* even);
    virtual void initializeGL();
    virtual void resizeGL(int _w, int _h);
    virtual void paintGL();

  private:
    /// last mouse position in window
    QPoint mousePosition_;

    static constexpr GLuint MAX_DISPLAY_LISTS=30000;*/
  };
}

#endif // GLWIDGET_H
