#pragma once

#include "common.hpp"

namespace tbd
{
  namespace gl
  {
    namespace traits
    {
      template<typename...ARGS>
      struct Vertex {};


#define TBD_GL_VERTEX_TRAITS(type,suffix)\
  TBD_GL_TRAITS(Vertex,glVertex2##suffix,type,type)\
  TBD_GL_TRAITS(Vertex,glVertex3##suffix,type,type,type)\
  TBD_GL_TRAITS(Vertex,glVertex4##suffix,type,type,type,type)
 
      TBD_GL_VERTEX_TRAITS(GLdouble,d)
      TBD_GL_VERTEX_TRAITS(GLfloat,f)
      TBD_GL_VERTEX_TRAITS(GLint,i)
      TBD_GL_VERTEX_TRAITS(GLshort,s)
    }

    template<typename...ARGS>
    void vertex(const ARGS&..._args)
    {
      traits::Vertex<ARGS...>()(_args...);
    }
  }
}
