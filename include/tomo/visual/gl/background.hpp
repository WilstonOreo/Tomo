#pragma once

#include <tomo/visual/gl.hpp>

namespace tomo
{
  namespace visual
  {
    namespace gl
    {
      template<typename COLOR_TOP, typename COLOR_BOTTOM>
      void background(const COLOR_TOP& _colorTop, const COLOR_BOTTOM& _colorBottom)
      {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        disabled(GL_DEPTH_TEST,[&]()
        {
          draw(GL_QUADS,[&]()
          {
            color(_colorTop);
            vertex(-1.0,-1.0);
            vertex(1.0,-1.0);
            color(_colorBottom);
            vertex(1.0, 1.0);
            vertex(-1.0, 1.0);
          });
        });

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
      }
    }
  }
}
