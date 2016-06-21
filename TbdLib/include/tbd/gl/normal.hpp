#pragma once

#include "common.hpp"

namespace tbd
{
  namespace gl
  {
    namespace traits 
    {
      template<typename...ARGS>
      struct Normal {};

#define TBD_GL_NORMAL_TRAITS(type,suffix)\
    TBD_GL_TRAITS(Normal,glNormal3##suffix,type,type,type)
      
      TBD_GL_NORMAL_TRAITS(GLfloat,f)
      TBD_GL_NORMAL_TRAITS(GLint,i)
      TBD_GL_NORMAL_TRAITS(GLbyte,b)
      TBD_GL_NORMAL_TRAITS(GLdouble,i)
      TBD_GL_NORMAL_TRAITS(GLshort,s)
    }

    template<typename...ARGS>
    void normal(const ARGS&..._args)
    {
      traits::Normal<ARGS...>()(_args...);
    }
  }
}
