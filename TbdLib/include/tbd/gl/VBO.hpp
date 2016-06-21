#pragma once

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include "Base.hpp"

namespace tomo
{
  namespace visual
  {
    namespace gl
    {
      enum class VBOType
      {
        Normal3f,
        Color4f,
        Vertex3f,
        Color4ub,
        Index
      };

      namespace detail
      {

        template<VBOType TYPE>
        struct VBOPointer {};

        template<>
        struct VBOPointer<VBOType::Normal3f>
        {
          static constexpr GLuint state() { return GL_NORMAL_ARRAY; }
          
          inline void operator()()
          {
            glNormalPointer(GL_FLOAT,0,nullptr);
          }
        };

        template<>
        struct VBOPointer<VBOType::Vertex3f>
        {
          static constexpr GLuint state() { return GL_VERTEX_ARRAY; }
          
          inline void operator()()
          {
            glVertexPointer(3,GL_FLOAT,0,nullptr);
          }
        };

        template<>
        struct VBOPointer<VBOType::Color4f>
        {
          static constexpr GLuint state() { return GL_COLOR_ARRAY; }

          inline void operator()()
          {
            glColorPointer(4,GL_FLOAT,0,nullptr);
          }
        };

        template<>
        struct VBOPointer<VBOType::Color4ub>
        {
          static constexpr GLuint state() { return GL_COLOR_ARRAY; }

          inline void operator()()
          {
            glColorPointer(4,GL_UNSIGNED_BYTE,0,nullptr);
          }
        };



        template<typename...VBOS>
        struct DrawVBO {};

        template<typename VBO>
        struct DrawVBO<VBO> 
        {
          DrawVBO(VBO const& _vbo) : vbo_(_vbo) {}

          void init() const
          {
            vbo_.init();
          }

          void enable() const 
          {
            vbo_.enable();
          }

          void disable() const
          {
            vbo_.disable();
          }

          template<GLuint PRIMITIVE>
          void draw() const
          {
            if (!vbo_.valid()) return;
            enable();
              glDrawArrays(PRIMITIVE,0,vbo_.size());
            disable();
          }
        private:
          VBO const& vbo_;
        };

        template<typename HEAD, typename...TAIL>
        struct DrawVBO<HEAD,TAIL...> :
          private DrawVBO<HEAD>,
          private DrawVBO<TAIL...>
        {
          typedef DrawVBO<HEAD> head_type;
          typedef DrawVBO<TAIL...> tail_type;
          
          DrawVBO(const HEAD& _head, const TAIL&..._tail) : 
            head_type(_head),
            tail_type(_tail...) {}

          void init() const
          {
            tail_type::init();
            head_type::init();
          }

          void enable() const 
          {
            tail_type::enable();
            head_type::enable();
          }

          void disable() const
          {
            tail_type::disable();
            head_type::disable();
          }

          template<GLuint PRIMITIVE>
          void draw() const
          {
            head_type::init();
            tail_type::init();

            tail_type::enable();
            head_type::template draw<PRIMITIVE>();
            tail_type::disable(); 
          }
        };
      }

      namespace vbo
      {
        template<GLuint PRIMITIVE, typename...ARGS>
        void draw(const ARGS&..._args)
        {
          detail::DrawVBO<ARGS...> _drawVBO(_args...);
          _drawVBO.template draw<PRIMITIVE>();
        }
      }

      template<VBOType TYPE>
      struct VBO
      {
        static constexpr VBOType type() { return TYPE; }

        VBO() : id_(0), size_(0)
        {
          glGenBuffers(1,&id_);
        }

        ~VBO()
        {
          if (id_)
          {
            glDeleteBuffers(1,&id_);
          }
        }

        template<GLuint MODE, typename T>
        void bufferData(const std::vector<T>& _data) 
        {
          size_ = _data.size();
          if (!valid()) return;
          bind();
          glBufferData(GL_ARRAY_BUFFER,size_*sizeof(_data.front()),_data.data(),MODE);
        }

        void init() const
        {
          if (!valid()) return;
          bind();
          detail::VBOPointer<TYPE>()();
        }

        void bind() const
        {
          if (!valid()) return;
          glBindBuffer(GL_ARRAY_BUFFER,id_);
        }

        bool valid() const
        {
          return size_ != 0;
        }

        void disable() const
        {
          if (!valid()) return;
          glDisableClientState(detail::VBOPointer<TYPE>::state());
        }
        void enable() const
        {
          if (!valid()) return;
          glEnableClientState(detail::VBOPointer<TYPE>::state());
        }

        TBD_PROPERTY_RO(GLuint,id)
        TBD_PROPERTY_RO(size_t,size)
      };

      typedef VBO<VBOType::Normal3f> VBONormal3f;
      typedef VBO<VBOType::Vertex3f> VBOVertex3f;
      typedef VBO<VBOType::Color4f> VBOColor4f;
      typedef VBO<VBOType::Color4ub> VBOColor4ub;
    }
  }
}
