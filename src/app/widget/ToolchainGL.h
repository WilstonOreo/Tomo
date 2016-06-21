#pragma once

#include <tomo/visual/gl.hpp>
#include <tomo/visual/Light.hpp>
#include <tomo/visual/Tracker.hpp>
#include <tomo/visual/Camera.hpp>
#include <tomo/visual/Interface.hpp>
#include <QtOpenGL/QGLWidget>

namespace tomo
{
  namespace widget
  {

    class ToolchainGL : public QGLWidget
    {
      Q_OBJECT
    public:
      typedef tomo::visual::Interface visual_interface_type;
      typedef gex::Point3 Point3;
      typedef gex::Vec3 Vec3;
      typedef gex::Scalar Scalar;
      typedef gex::Bounds3 Bounds3;
      typedef gex::base::PolarVec<float> PolarVec;
      typedef visual::Tracker<float,false,true> LightTracker;
      typedef visual::Tracker<float,true,true> CameraTracker;
      typedef visual::Light<LightTracker,Color4> Light;
      typedef visual::Camera<CameraTracker> Camera;

      explicit ToolchainGL(QWidget* _parent = nullptr);
      ~ToolchainGL();

      bool empty() const;
      void setDevice(visual_interface_type* _device);

      Bounds3 bounds() const;

      Light* light();
      Light const* light() const;

      float aspect() const;

      TBD_PROPERTY_RO(visual_interface_type*,device)
      TBD_PROPERTY_REF_RO(visual::VisualizerList,items)

      TBD_PROPERTY_REF(float,cameraFOV)
      TBD_PROPERTY_REF(Camera,camera)
      TBD_PROPERTY_REF(Light,light0)
      TBD_PROPERTY_REF(Light,light1)
      TBD_PROPERTY_REF(Light,light2)
      TBD_PROPERTY(int,selectedLight)
      TBD_PROPERTY_REF(Color4,bgrColorTop)
      TBD_PROPERTY_REF(Color4,bgrColorBottom)
      TBD_PROPERTY_REF_RO(QPoint,mousePosition)
    public slots:
      void setItems(tomo::visual::VisualizerList);
    
    protected:
      void initializeGL();
      void resizeGL(int _w, int _h);
      void paintGL();
      void mouseMoveEvent(QMouseEvent *event);
      void wheelEvent(QWheelEvent *event);
      void mousePressEvent(QMouseEvent *event);
      void keyPressEvent(QKeyEvent* even);


    private:
      Light* light(int _lightIndex);
      Light const* light(int _lightIndex) const;
      void updateLight(int _lightIndex);
      void drawLights();
      void setupLight(int _lightIndex, const Point3& _center, const Scalar& _radius );
    };
  }
}
