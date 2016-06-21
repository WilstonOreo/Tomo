#pragma once

#include "common.hpp"

namespace tbd
{
  namespace gl
  {
    namespace traits
    {
      template<typename...ARGS>
      struct TexCoord {};

#define TBD_GL_TEXCOORD_TRAITS(type,suffix)\
      TBD_GL_TRAITS(TexCoord,glTexCoord1##suffix,type)\
      TBD_GL_TRAITS(TexCoord,glTexCoord2##suffix,type,type)\
      TBD_GL_TRAITS(TexCoord,glTexCoord3##suffix,type,type,type)\
      TBD_GL_TRAITS(TexCoord,glTexCoord4##suffix,type,type,type,type)

      TBD_GL_TEXCOORD_TRAITS(GLfloat,f)
      TBD_GL_TEXCOORD_TRAITS(GLint,i)
      TBD_GL_TEXCOORD_TRAITS(GLdouble,d)
      TBD_GL_TEXCOORD_TRAITS(GLshort,s)
    }

    template<typename...ARGS>
    void texCoord(const ARGS&..._args)
    {
      traits::TexCoord<ARGS...>()(_args...);
    }
  }
}
