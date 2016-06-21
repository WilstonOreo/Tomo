#pragma once

#include <tomo/visual/gl.hpp>

namespace tomo
{
  namespace visual
  {
    namespace gl
    {
      template<typename BOUNDS, typename COLOR>
      void bounds(const BOUNDS& _bounds,
                      const COLOR& _color)
      {
        float x = _bounds.min().x(), y = _bounds.min().y(), z = _bounds.min().z();
        float xs = _bounds.max().x(), ys = _bounds.max().y(), zs = _bounds.max().z();
        color(_color);
        glBegin(GL_LINE_LOOP);
        {
          // top side
          glVertex3f(x , ys, z );
          glVertex3f(xs, ys, z );
          glVertex3f(xs, ys, zs);
          glVertex3f(x , ys, zs);
        }
        glEnd();
        glBegin(GL_LINE_LOOP);
        {
          // bottom side
          glVertex3f(x , y, z );
          glVertex3f(xs, y, z );
          glVertex3f(xs, y, zs);
          glVertex3f(x , y, zs);
        }
        glEnd();
        glBegin(GL_LINE_LOOP);
        {
          // east side
          glVertex3f(x , y , z);
          glVertex3f(xs, y , z);
          glVertex3f(xs, ys, z);
          glVertex3f(x , ys, z);
        }
        glEnd();
        glBegin(GL_LINE_LOOP);
        // west side
        {
          glVertex3f(x , y ,zs);
          glVertex3f(xs, y ,zs);
          glVertex3f(xs, ys,zs);
          glVertex3f(x , ys,zs);
        }
        glEnd();
        glBegin(GL_LINE_LOOP);
        {
          // north side
          glVertex3f(x , y , z );
          glVertex3f(x , y , zs);
          glVertex3f(x , ys, zs);
          glVertex3f(x , ys, z );
        }
        glEnd();
        glBegin(GL_LINE_LOOP);
        {
          // south side
          glVertex3f( xs, y , z );
          glVertex3f( xs, y , zs);
          glVertex3f( xs, ys, zs);
          glVertex3f( xs, ys, z );
        }
        glEnd();
      }
    }
  }
}

