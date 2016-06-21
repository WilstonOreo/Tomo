#pragma once

#include <tbd/property.h>

namespace tbd
{
  namespace gl
  {
    namespace detail
    {
      template<typename ELEMENT>
      struct VBOInitializer
      {
      };
    }


    template<GLuint TYPE, typename ELEMENT>
    struct IndexedVBO
    {
      static constexpr GLuint type()
      {
        return TYPE;
      }

      template<typename ARRAY, typename ELEMENT_ARRAY>
      void initialize(const ARRAY_BUFFER& _geom, const ELEMENT_ARRAY& _indices)
      {
        glEnableClientState(GL_VERTEX_ARRAY);
        glGenBuffers(1,bufferId_);
        glBindBuffer(GL_ARRAY_BUFFER,bufferId_);
        glBufferData(GL_ARRAY_BUFFER,sizeof(_geom.data()),_geom.data());
        


        glGenBuffers(1,indexBufferId_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexBufferId_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(_indices.data()),_indices.data());

        detail::VBOInitializer<element_type>()(_indices);


      }

      void bind()
      {
        gl 
      }

      template<typename BEGIN, typename END>
      void render(BEGIN _begin, END _end)
      {
        glDrawArrays(type(),0,_end - _begin);
      }

      TBD_PROPERTY_RO(GLuint,indexBufferId)
      TBD_PROPERTY_RO(GLuint,bufferId)
    };

    template<GLuint TYPE, typename VBO>
    void render_vbo(
    
  }
}
