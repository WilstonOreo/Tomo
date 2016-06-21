#include "widget/ToolchainGL.h"

#include <tomo/visual/Interface.hpp>
#include <tomo/visual/gl/background.hpp>

#include <QMouseEvent>

namespace tomo
{
  namespace widget
  {
    ToolchainGL::ToolchainGL(QWidget* _parent) :
      QGLWidget(
        QGLFormat(
          QGL::DoubleBuffer |
          QGL::DepthBuffer |
          QGL::Rgba |
          QGL::AlphaChannel |
          QGL::DirectRendering),_parent),
      device_(nullptr),
      cameraFOV_(60.0),
      selectedLight_(0),
      bgrColorTop_(Color3(0,0,0)),
      bgrColorBottom_(Color3(0.3,0.4,0.5)),
      mousePosition_(0,0)
    {
      makeCurrent();
    // No glew required for apple
#ifndef __APPLE__ 
      //glewExperimental=GL_TRUE; // Enable experimental extension for OpenGL 3.3 and later  
      static GLenum err = -1;
      if (err == -1)
      {
        err = glewInit(); 
        if (GLEW_OK != err)
        {
          std::cerr << "GLEW could not be initialized. Error: " << glewGetErrorString(err) << std::endl;
        } else
        {
          std::cerr << "GLEW was initialized!" << std::endl;
        }
      }
#endif
    }

    ToolchainGL::~ToolchainGL()
    {
    }

    bool ToolchainGL::empty() const
    {
      return !device() && items_.empty();
    }

    ToolchainGL::Bounds3 ToolchainGL::bounds() const
    {
      if (device())
      {
        return device()->bounds();
      }

      Bounds3 _bounds;

      for (auto& _item : items_)
        _bounds.extend(_item->bounds());

      return _bounds;
    }

    void ToolchainGL::initializeGL()
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable(GL_LINE_SMOOTH);
      glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
      glEnable(GL_POINT_SMOOTH);
      glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
      glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
      glPolygonMode(GL_FRONT,GL_FILL);
      glPolygonMode(GL_BACK,GL_FILL);
      glEnable(GL_NORMALIZE);
      // fix outlines z-fighting with quads
      glPolygonOffset(1, 1);
        
      auto _bounds = bounds();
      auto _center = Point3(Point3(
                              _bounds.center().x(),
                              _bounds.center().y(),
                              _bounds.min().z()));
      Scalar _radius = _bounds.radius();

      // setup camera
      camera_ = Camera(CameraTracker(
                         // target to track (origin)
                         _center,
                         // set tracking position from spheric coordinates
                         PolarVec(-45.0, 45.0, _radius*2.0 )
                       ),
                       // near, far
                       0.01, 1000.0,
                       Vec3(0.0, 0.0, 1.0)
                      );

      for (int i = 0; i< 3; i++)
        setupLight(i,_center,_radius);
    }

    void ToolchainGL::setDevice(visual_interface_type* _device)
    {
      if (_device && (_device != device())) 
      {
        device_=_device;
        initializeGL();
      }
      device_=_device;
    }

    void ToolchainGL::setItems(visual::VisualizerList  _items)
    {
      if (items_.empty() && !_items.empty() && !device())
      {
        items_ = _items;
        initializeGL();
        return;
      }
      items_ = _items;
    }

    void ToolchainGL::resizeGL(int _w, int _h)
    {
      // restore view definition after window reshape
      //  perspective projection
      glClearColor(0.0,0.0,0.0,1.0);
      glViewport(0, 0, (GLint)_w, (GLint)_h);

      update();
    }

    float ToolchainGL::aspect() const
    {
      return float(width()) / height();
    }

    void ToolchainGL::paintGL()
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      using namespace tomo::visual;
      glDisable(GL_LIGHTING);
      gl::background(bgrColorTop_,bgrColorBottom_);
      if (empty()) return;

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(cameraFOV(), aspect(),
                     camera().near(), camera().far());

      // realize coordinates
      gluLookAt(
        camera_.eye().x(),
        camera_.eye().y(),
        camera_.eye().z(),
        camera_.center().x(),
        camera_.center().y(),
        camera_.center().z(),
        camera_.up().x(),
        camera_.up().y(),
        camera_.up().z()
      );

      glMatrixMode(GL_MODELVIEW);
      glEnable(GL_DEPTH_TEST);
      glLoadIdentity();
      glPushMatrix();

      drawLights();
      if (device())
      {
        device()->process();
      }

      for (auto& _item : items_)
      {
        _item->process();
      }

      glPopMatrix();

    }

    void ToolchainGL::updateLight(int _lightIndex)
    {
      Light* _light = light(_lightIndex);
      if (!_light) return;
      GLuint _glIndex = GL_LIGHT1 + _lightIndex;

      using namespace gex::base;
      gex::Vec4 _pos;
      _pos(X) = _light->eye().x();
      _pos(Y) = _light->eye().y();
      _pos(Z) = _light->eye().z();
      _pos(W) = 0.0;

      // light and material
      glEnable(GL_LIGHTING);
      glEnable(GL_COLOR_MATERIAL);
      glLightModelfv(GL_LIGHT_MODEL_AMBIENT, _light->ambient());
      glLightfv(_glIndex, GL_POSITION, _pos.data());
      glLightfv(_glIndex, GL_DIFFUSE, _light->diffuse());
      glLightfv(_glIndex, GL_AMBIENT, _light->ambient());
      glEnable(_glIndex);
    }


    void ToolchainGL::setupLight(int _lightIndex, const Point3& _center, const Scalar& _radius )
    {
      Light* _light = light(_lightIndex);
      if (!_light) return;

      float _intensity = 1.0 / (_lightIndex + 1);

      // setup light source
      *_light = Light(
                  LightTracker(
                    // target to track (origin)
                    _center,
                    // set tracking position from spheric coordinates
                    PolarVec(120 * _lightIndex, 45.0 * _lightIndex, _radius )
                  ),
                  Color3(0.1,0.1,0.1) * _intensity, 
                  Color3(0.7,0.7,0.7) * _intensity, 
                  Color3(0.2,0.2,0.2) * _intensity,
                  // intensity
                  1.0,
                  // shadows
                  0.0,
                  // radius
                  1.0
                );

      // light and material
      updateLight(_lightIndex);
    }
    
    ToolchainGL::Light* ToolchainGL::light(int _index )
    {
      switch (_index)
      {
      case 0:
        return &light0();
      case 1:
        return &light1();
      case 2:
        return &light2();
      default:
        return nullptr;
      }
      return nullptr;
    }

    ToolchainGL::Light const* ToolchainGL::light(int _index ) const
    {
      switch (_index)
      {
      case 0:
        return &light0();
      case 1:
        return &light1();
      case 2:
        return &light2();
      default:
        return nullptr;
      }
      return nullptr;
    }

    ToolchainGL::Light* ToolchainGL::light()
    {
      return light(selectedLight_);
    }

    ToolchainGL::Light const* ToolchainGL::light() const
    {
      return light(selectedLight_);
    }

    void ToolchainGL::drawLights()
    {
      using namespace tbd::gl;

      glPointSize(5.0);
      draw(GL_POINTS,[&]()
      {
        for (int i = 0; i< 3; i++)
        {
          Light* _light = light(i);
          color( _light->diffuse() );
          vertex( _light->eye() );
        }
      });
    }

    void ToolchainGL::mouseMoveEvent(QMouseEvent *event)
    {
      if (event->buttons() & Qt::LeftButton)
      {
        if( event->modifiers() & Qt::ShiftModifier )
        {
          camera_.strafe((event->pos().x() - mousePosition().x())/2.0);
          camera_.lift((event->pos().y() - mousePosition().y())/2.0);
        }
        else
        {
          if( event->modifiers() & Qt::ControlModifier )
          {
            if (!light()) return;
            light()->track( event->pos().x() - mousePosition().x(), - event->pos().y() + mousePosition().y(), 0 );
            updateLight(selectedLight_);
          }
          if( !(event->modifiers() & Qt::ControlModifier) )
            camera_.track( event->pos().x() - mousePosition().x(), event->pos().y() - mousePosition().y(), 0 );
        }
        update();
      }
      mousePosition_ = event->pos();
    }
    void ToolchainGL::wheelEvent(QWheelEvent *event)
    {
      camera_.track( 0, 0, (double)event->delta()/20.0 );
      update();
    }

    void ToolchainGL::mousePressEvent(QMouseEvent *event)
    {
      switch (event->button())
      {
      case Qt::LeftButton:
        //selection_ = unProject(event->pos()); ///@todo selection by click
        mousePosition_ = event->pos();
        update();
        break;
      default:
        ;
      }
    }

    void ToolchainGL::keyPressEvent(QKeyEvent* _event)
    {
      int step = 1;
      if( _event->modifiers() & Qt::ShiftModifier )
        step *= 10;
      int x=0, y=0, z=0;
      switch( _event->key() )
      {
      case Qt::Key_PageUp:
        z = -step;
        break;
      case Qt::Key_Up:
        y = -step;
        break;
      case Qt::Key_Left:
        x = -step;
        break;
      case Qt::Key_PageDown:
        z = step;
        break;
      case Qt::Key_Down:
        y = step;
        break;
      case Qt::Key_Right:
        x = step;
        break;
      }


      if( _event->modifiers() & Qt::ControlModifier )
      {
        if (!light()) return;
        light()->track( x,y,z );
        updateLight(selectedLight_);
      }
      if( !(_event->modifiers() & Qt::ControlModifier) )
        camera_.track( x,y,z );

      QGLWidget::keyPressEvent(_event);
      update();
    }

  }
}
