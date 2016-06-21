#pragma once

#include <iostream>

namespace tbd
{
  namespace gl
  {
    struct FrameBufferTexture
    {
      FrameBufferTexture() :
        tex_(0),
        fbo_(0),
        renderBuf_(0)
      {
      }

      FrameBufferTexture(int _width, int _height) :
        tex_(0),
        fbo_(0),
        renderBuf_(0)
      {
        initialize(_width,_height);
      }

      ~FrameBufferTexture()
      {
        free();
      }

      bool valid() const
      {
        return fbo_ != 0;
      }

      void free()
      {
        glDeleteTextures(1,&tex_);
        glDeleteFramebuffers(1,&fbo_);
        glDeleteRenderbuffers(1,&renderBuf_);

        tex_ = 0;
        fbo_ = 0;
        renderBuf_ = 0;
      }

      void initialize(int _width, int _height)
      {
        width_=_width;
        height_=_height;

        if (!valid())
        {
          glGenFramebuffers(1,&fbo_);
          glGenTextures(1,&tex_);
          glGenFramebuffers(1,&renderBuf_);
        }

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,tex_);
        {
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
          glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,_width,_height,0,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
        }
        glBindTexture(GL_TEXTURE_2D,0);

        glBindFramebuffer(GL_FRAMEBUFFER,fbo_);

        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,tex_,0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,renderBuf_);

        glBindRenderbuffer(GL_RENDERBUFFER,renderBuf_);
        glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,_width,_height);
        glBindRenderbuffer(GL_RENDERBUFFER,0);

        glBindFramebuffer(GL_FRAMEBUFFER,0);

        GLenum status;
        status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        switch(status)
        {
        case GL_FRAMEBUFFER_COMPLETE:
          std::cout << "fbo complete" << std::endl;
          break;

        case GL_FRAMEBUFFER_UNSUPPORTED:
          std::cout << "fbo unsupported" << std::endl;
          break;

        default:
          /* programming error; will fail on all hardware */
          std::cout << "fbo error" << std::endl;
          break;
        }

      }

      void bind() const
      {
        glBindFramebuffer(GL_FRAMEBUFFER,fbo_);
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glViewport(0,0, width(), height());

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      }

      static void release()
      {
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glPopAttrib();
      }

      static GLuint check()
      {
        return glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
      }

      TBD_PROPERTY_RO(int,width)
      TBD_PROPERTY_RO(int,height)
      TBD_PROPERTY_RO(GLuint,tex)
    private:
      GLuint fbo_, renderBuf_;
    };

    template<typename FRAMEBUFFER, typename FUNCTOR>
    void draw_on_framebuffer(FRAMEBUFFER const& _frameBuffer, FUNCTOR _f)
    {
      _frameBuffer.bind();
      _f();
      _frameBuffer.release();
    };

    template<typename FRAMEBUFFER, typename FUNCTOR>
    void draw_with_framebuffer_texture(FRAMEBUFFER const& _frameBuffer, FUNCTOR _f)
    {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,_frameBuffer.tex());
      _f();
      glBindTexture(GL_TEXTURE_2D,0);
      glDisable(GL_TEXTURE_2D);
    };
  }
}
