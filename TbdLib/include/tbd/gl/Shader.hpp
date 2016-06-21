#pragma once

#include <string>
#include "Texture.hpp"

namespace tbd
{
  namespace gl
  {
    namespace traits 
    {
      template<typename...ARGS>
      struct Uniform
        {};

      /// glUniform1f handler
      template<>
      struct Uniform<GLfloat>
      {
        template<typename...ARGS>
        void operator()(GLint _loc, ARGS&&..._args)
        {
          glUniform1f(_loc,_args...);
        }
      };

      /// glUniform2f handler
      template<>
      struct Uniform<GLfloat,GLfloat>
      {
        template<typename...ARGS>
        void operator()(GLint _loc, ARGS&&..._args)
        {
          glUniform2f(_loc,_args...);
        }
      };

      /// glUniform3f handler
      template<>
      struct Uniform<GLfloat,GLfloat,GLfloat>
      {
        template<typename...ARGS>
        void operator()(GLint _loc, ARGS&&..._args)
        {
          glUniform3f(_loc,_args...);
        }
      };


      /// glUniform4f handler
      template<>
      struct Uniform<GLfloat,GLfloat,GLfloat,GLfloat>
      {
        template<typename...ARGS>
        void operator()(GLint _loc, ARGS&&..._args)
        {
          glUniform4f(_loc,_args...);
        }
      };

      /// glUniform1i handler
      template<>
      struct Uniform<GLint>
      {
        template<typename...ARGS>
        void operator()(GLint _loc, ARGS&&..._args)
        {
          glUniform1i(_loc,_args...);
        }
      };

      template<typename IMAGE, GLuint TYPE>
      struct Uniform<Texture<IMAGE,TYPE>>
      {
        template<typename T>
        void operator()(GLint _loc, const T& _t)
        {
          _t.bind();
          glActiveTexture(GL_TEXTURE0);
          glUniform1i(_loc,0);
          //  _t.release();
        }
      };

      template<typename T>
      struct Uniform<T const&> :
          Uniform<T>
        {};

      template<typename T>
      struct Uniform<T&> :
          Uniform<T>
        {};

      /// glUniform2i handler
      template<>
      struct Uniform<GLint,GLint>
      {
        template<typename...ARGS>
        void operator()(GLint _loc, ARGS&&..._args)
        {
          glUniform2i(_loc,_args...);
        }
      };

      /// glUniform3i handler
      template<>
      struct Uniform<GLint,GLint,GLint>
      {
        template<typename...ARGS>
        void operator()(GLint _loc, ARGS&&..._args)
        {
          glUniform3i(_loc,_args...);
        }
      };

      /// glUniform4i handler
      template<>
      struct Uniform<GLint,GLint,GLint,GLint>
      {
        template<typename...ARGS>
        void operator()(GLint _loc, ARGS&&..._args)
        {
          glUniform4i(_loc,_args...);
        }
      };
    }

    template<GLuint TYPE>
    struct Shader
    {
      Shader() : id_(0) {}

      static constexpr GLuint type() { return TYPE; }

      void initialize(std::string const& _src)
      {
        if (id_) return; // Shader is already initialized
        id_ = glCreateShader(TYPE);
        const char* _s = _src.c_str();
        glShaderSource(id_,1,_s,nullptr);
      }

      void compile() const
      {
        glCompileShader(id_);
      }

      void free() const
      {
        glDeleteShader(id_);
      }

      void attach(GLuint _programId) const
      {
        glAttachShader(_programId,id_);
      }

      void detach(GLuint _programId) const
      {
        glDetachShader(_programId,id_);
      }

    private:
      GLuint id_;
    };

    namespace
    {

      template<GLuint...TYPE>
      struct ShaderHandler {};
    
      template<GLuint TYPE>
      struct ShaderHandler<TYPE> : public Shader<TYPE>
      {
        static constexpr GLuint type() { return TYPE; }
        typedef Shader<type()> shader_type;
      
        ShaderHandler() {}
        ShaderHandler(std::string const& _str) : 
          shader_type(_str) {}
      };

      template<GLuint TYPE, GLuint...TYPES>
      struct ShaderHandler<TYPE,TYPES...> : 
        private ShaderHandler<TYPE>,
        private ShaderHandler<TYPES...>
      {
      private:
        typedef ShaderHandler<TYPE> head_type;
        typedef ShaderHandler<TYPES...> tail_type;
      public:
        ShaderHandler() {}

        template<typename...ARGS>
        ShaderHandler(std::string const& _arg, ARGS const&..._args) 
        {
          initialize(_arg,_args...);
        }

        void compile() const
        {
          head_type::compile();
          tail_type::compile();
        }

        template<typename...ARGS>
        void initialize(std::string const& _src, const ARGS&..._args)
        {
          head_type::initialize(_src);
          tail_type::initialize(_args...);
        }

        void free() const
        {
          head_type::free();
          tail_type::free();
        }

        void attach(GLuint _programId) const
        {
          head_type::attach(_programId);
          tail_type::attach(_programId);
        }

        void detach(GLuint _programId) const
        {
          head_type::detach(_programId);
          tail_type::detach(_programId);
        }
      };
    }

    template<GLuint...TYPE>
    struct ShaderProgram : private ShaderHandler<TYPE...>
    {
      ShaderProgram() :
        id_(0) {}

      typedef ShaderHandler<TYPE...> handler_type;

      template<typename...ARGS>
      ShaderProgram(const ARGS&..._args) : 
        id_(0)
      {
        initialize(_args...);
      }

      ~ShaderProgram()
      {
        free();
      }

      void free()
      {
        handler_type::detach(id_);
        handler_type::free();
        glDeleteProgram(id_);
      }

      bool valid() const
      {
        return id_ != 0;
      }

      operator bool() const
      {
        return valid();
      }


      template<typename...ARGS>
      void initialize(const ARGS&..._args)
      {
        id_ = glCreateProgram();
        handler_type::initialize(_args...);
        handler_type::attach(id_);
        handler_type::compile();
        glLinkProgram(id_);
      }

      void bind() const
      {
        glUseProgram(id_);
      }

      static void release()
      {
        glUseProgram(0);
      }

      template<typename...ARGS>
      ShaderProgram const& uniform(const char* _name, ARGS&&..._args) const
      {
        GLint _loc = glGetUniformLocation(id_,_name);
        if (_loc == GL_INVALID_VALUE) return *this;
        traits::Uniform<ARGS...>()(_loc,_args...);
        return *this;
      }

      template<typename...ARGS>
      ShaderProgram const& operator()(const char* _name, ARGS&&..._args) const
      {
        return this->uniform(_name,_args...);
      }

      TBD_PROPERTY_REF_RO(GLuint,id)
    };

    template<typename SHADERPROGRAM, typename FUNCTOR>
    void use_shader(SHADERPROGRAM const& _shader, FUNCTOR _f)
    {
      _shader.bind();
      _f();
      _shader.release();
    };
  }
}

