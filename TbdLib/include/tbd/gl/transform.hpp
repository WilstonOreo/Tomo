#pragma once

#include "common.hpp"

namespace tbd
{
  namespace gl
  {
    namespace traits
    {
      template<typename...ARGS>
      struct Translate {};
      
      template<typename...ARGS>
      struct Rotate {};
      
      template<typename...ARGS>
      struct Scale {};
      
      template<>
      struct Rotate<GLdouble,GLdouble,GLdouble,GLdouble>
      {
        TBD_GL_TRAITS_CALL(glRotated)
      };
      
      template<>
      struct Rotate<GLfloat,GLfloat,GLfloat,GLfloat>
      {
        TBD_GL_TRAITS_CALL(glRotatef)
      };

      template<>
      struct Translate<GLdouble,GLdouble,GLdouble>
      {
        TBD_GL_TRAITS_CALL(glTranslated)
      };
      
      template<>
      struct Translate<GLfloat,GLfloat,GLfloat>
      {
        TBD_GL_TRAITS_CALL(glTranslatef)
      };

      template<>
      struct Scale<GLfloat,GLfloat,GLfloat>
      {
        TBD_GL_TRAITS_CALL(glScalef)
      };
      
      template<>
      struct Scale<GLdouble,GLdouble,GLdouble>
      {
        TBD_GL_TRAITS_CALL(glScaled)
      };
    }

    template<typename...ARGS>
    void translate(const ARGS&..._args)
    {
      traits::Translate<ARGS...>()(_args...);
    }
    template<typename...ARGS>
    void rotate(const ARGS&..._args)
    {
      traits::Rotate<ARGS...>()(_args...);
    }
    template<typename...ARGS>
    void scale(const ARGS&..._args)
    {
      traits::Scale<ARGS...>()(_args...);
    }

    template<typename F>
    void push_matrix(F _f)
    {
      glPushMatrix();
      _f();
      glPopMatrix();
    }
  }
}
