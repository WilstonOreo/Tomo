#pragma once
#define GL_GLEXT_PROTOTYPES

#ifdef __APPLE__
//#include <OpenGL/glew.h>
#define QT_NO_OPENGL_ES_2
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glew.h>
#define QT_NO_OPENGL_ES_2
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <tbd/gl.hpp>
#include <gex/base.hpp>
#include "Color.hpp"

namespace tomo
{
  namespace visual
  {
    namespace gl
    {
      using namespace tbd::gl;
    }
  }
}
namespace tbd
{
  namespace gl
  {
    namespace traits
    {
      template<>
      struct Vertex<gex::Vec2>
      {
        template<typename T>
        void operator()(const T& _t)
        {
          glVertex2f(_t.x(),_t.y());
        }
      };
      
      template<>
      struct Vertex<gex::Vec3>
      {
        template<typename T>
        void operator()(const T& _t)
        {
          glVertex3f(_t.x(),_t.y(),_t.z());
        }
      };
      
      template<>
      struct Normal<gex::Vec3>
      {
        template<typename T>
        void operator()(const T& _t)
        {
          glNormal3f(_t.x(),_t.y(),_t.z());
        }
      };

      template<>
      struct Color<tomo::Color4>
      {
        template<typename T>
        void operator()(const T& _c)
        {
          glColor4f(_c.r(),_c.g(),_c.b(),_c.a());
        }
      };
      template<>
      struct Color<tomo::Color3>
      {
        template<typename T>
        void operator()(const T& _c)
        {
          glColor3f(_c.r(),_c.g(),_c.b());
        }
      };
    
      template<>
      struct Translate<gex::Vec3>
      {
        template<typename T>
        void operator()(const T& _t)
        {
          tbd::gl::translate(_t.x(),_t.y(),_t.z());
        }
      };
      
      template<>
      struct Scale<gex::Vec3>
      {
        template<typename T>
        void operator()(const T& _t)
        {
          tbd::gl::scale(_t.x(),_t.y(),_t.z());
        }
      };
 
      template<>
      struct Rotate<gex::Scalar,gex::Axis>
      {
        template<typename T, typename AXIS>
        void operator()(const T& _c, const AXIS& _axis)
        {
            tbd::gl::rotate(GLfloat(_c),
                GLfloat(_axis == gex::X),
                GLfloat(_axis == gex::Y),
                GLfloat(_axis == gex::Z));
        }
      };
    }
  }
}

