#pragma once

namespace tbd
{
  namespace gl
  {
    namespace traits
    {
      template<typename...ARGS>
      struct Color {};

#define TBD_GL_COLOR_TRAITS(type,suffix)\
      TBD_GL_TRAITS(Color,glColor3##suffix,type,type,type)\
      TBD_GL_TRAITS(Color,glColor4##suffix,type,type,type,type)

      TBD_GL_COLOR_TRAITS(GLbyte,b)
      TBD_GL_COLOR_TRAITS(GLdouble,d)
      TBD_GL_COLOR_TRAITS(GLfloat,f)
      TBD_GL_COLOR_TRAITS(GLint,i)
      TBD_GL_COLOR_TRAITS(GLshort,s)
      TBD_GL_COLOR_TRAITS(GLubyte,ub)
      TBD_GL_COLOR_TRAITS(GLuint,ui)
      TBD_GL_COLOR_TRAITS(GLushort,us)
    }

    template<typename...ARGS>
    void color(ARGS const&..._args)
    {
      traits::Color<ARGS...>()(_args...);
    }
  }
}
