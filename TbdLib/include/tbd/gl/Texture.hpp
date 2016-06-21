#pragma once

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace tbd
{
  namespace gl
  {
    namespace traits 
    {
      /*  template<typename TEXTURE>
        struct TextureBinder
        {
          template<typename I>
          void operator()(const I& _image)
          {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_REPEAT );
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_REPEAT );
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TEXTURE::magFilter());
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TEXTURE::minFilter());
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
          }
        };*/

      template<typename IMAGE, GLuint TYPE>
      struct TextureInitializer
      {
        template<typename TEXTURE>
        void operator()(const TEXTURE& _tex)
        {
          glTexImage2D( TYPE, 0, GL_RGBA, _tex.width(), _tex.height(),0,
                        GL_RGBA, GL_FLOAT, (GLvoid const*)_tex.image()->data().data());
        }
      };
    }

    template<typename IMAGE, GLuint TYPE = GL_TEXTURE_2D>
    struct Texture
    {
      typedef IMAGE image_type;
      static constexpr GLuint type()
      {
        return TYPE;
      }

      Texture() :
        id_(0),
        image_(nullptr)
      {
      }

      Texture(image_type const* _image) :
        image_(_image)
      {
      }

      static void mipmap()
      {
        glTexParameteri(type(), GL_GENERATE_MIPMAP, GL_TRUE);
      }

      static void wrap(GLuint _s, GLuint _t)
      {
        glTexParameteri(type(), GL_TEXTURE_WRAP_S,  _s);
        glTexParameteri(type(), GL_TEXTURE_WRAP_T,  _t);
      }

      static void min_mag(GLuint _min, GLuint _mag)
      {
        glTexParameteri(type(), GL_TEXTURE_MIN_FILTER,  _min);
        glTexParameteri(type(), GL_TEXTURE_MAG_FILTER,  _mag);
      }

      ~Texture()
      {
        free();
      }

      bool initialize()
      {
        if (!image_) return false;
        return initialize(image_);
      }

      void free()
      {
        if (id_) glDeleteTextures(1,&id_);
        id_ = 0;
      }

      bool initialize(image_type const* _image)
      {
        image_=_image;
        if (!id_) glGenTextures(1,&id_);
        bind();
        traits::TextureInitializer<image_type,type()>()(*this);
        release();
        return true;
      }

      void bind() const
      {
        glEnable(type());
        glBindTexture(type(), id_);
      }

      int width() const
      {
        return image_->width();
      }

      int height() const
      {
        return image_->height();
      }

      static void release()
      {
        glBindTexture(type(), 0);
        glDisable(type());
      }

      image_type const* image() const
      {
        return image_;
      }

      TBD_PROPERTY_RO(GLuint,id)
    private:
      image_type const* image_;
    };

    template<typename T, typename F>
    void use_texture(const T& _tex, F f)
    {
      _tex.bind();
      f();
      _tex.release();
    }
  }
}
