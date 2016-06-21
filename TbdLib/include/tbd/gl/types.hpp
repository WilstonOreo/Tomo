#pragma once

namespace tbd
{
  namespace gl
  {
    template<GLuint ENUM> struct Type {};
    template<typename T> struct TypeId {};

#define TBD_GL_TYPE(gl_type,common_enum)\
    template<>\
    struct TypeId<gl_type>\
    {\
      inline static constexpr GLuint id()\
      {\
        return common_enum;\
      }\
    };\
    template<>\
    struct Type<common_enum>\
    {\
      typedef gl_type type;\
    };

    TBD_GL_TYPE(GLbyte,GL_BYTE)
    TBD_GL_TYPE(GLubyte,GL_UNSIGNED_BYTE)
    TBD_GL_TYPE(GLshort,GL_SHORT)
    TBD_GL_TYPE(GLushort,GL_UNSIGNED_SHORT)
    TBD_GL_TYPE(GLint,GL_INT)
    TBD_GL_TYPE(GLuint,GL_UNSIGNED_INT)
    TBD_GL_TYPE(GLfloat,GL_FLOAT)
    TBD_GL_TYPE(GLdouble,GL_DOUBLE)

    template<typename T>
    GLuint type_id()
    {
      return TypeId<T>::id();
    }
 
  }
}
