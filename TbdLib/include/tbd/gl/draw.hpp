#pragma once

namespace tbd
{
  namespace gl
  {
    template<typename F>
    void draw(GLuint _type, F f)
    {
      glBegin(_type);
      f();
      glEnd();
    }
  }
}
