#pragma once

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <gex/base.hpp>

namespace tomo
{
  namespace visual
  {
    template<typename POINT2>
    gex::Point3 unProject(const POINT2& pos)
    {
      GLdouble projection[16];
      glGetDoublev(GL_PROJECTION_MATRIX, projection);
      GLdouble modelView[16];
      glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
      GLint viewport[4];
      glGetIntegerv(GL_VIEWPORT, viewport);
      double winX = pos.x();
      double winY = viewport[3] - pos.y();
      GLfloat winZ;
      glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
      GLdouble x,y,z;
      gluUnProject(winX, winY, winZ, modelView, projection, viewport, &x, &y, &z);
      return gex::Point3(x,y,z);
    }
  }
}
