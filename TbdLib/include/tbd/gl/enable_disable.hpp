#pragma once

namespace tbd
{
  namespace gl
  {
    namespace detail
    {
      template<typename...ARGS>
      struct Enabler
      {};

      template<typename ARG>
      struct Enabler<ARG>
      {
        static void enable(GLuint _switch)
        {
          glEnable(_switch);
        };
      };

      template<typename ARG, typename...ARGS>
      struct Enabler<ARG,ARGS...>  
      {
        typedef Enabler<ARG> head_type;
        typedef Enabler<ARGS...> tail_type;

        template<typename T, typename...TS>
        static void enable(T&&_arg,TS&&..._args)
        {
          head_type::enable(_arg);
          tail_type::enable(_args...);
        }
      };

      template<typename...ARGS>
      struct Disabler
      {};

      template<typename ARG>
      struct Disabler<ARG>
      {
        static void disable(GLuint _switch)
        {
          glDisable(_switch);
        };
      };

      template<typename ARG, typename...ARGS>
      struct Disabler<ARG,ARGS...>  
      {
        typedef Disabler<ARG> head_type;
        typedef Disabler<ARGS...> tail_type;

        template<typename T, typename...TS>
        static void disable(T&&_arg,TS&&..._args)
        {
          head_type::disable(_arg);
          tail_type::disable(_args...);
        }
      };
    }

    template<typename...ARGS>
    void enable(ARGS&&..._args)
    {
      detail::Enabler<ARGS...>::enable(_args...);
    }
    
    template<typename...ARGS>
    void disable(ARGS&&..._args)
    {
      detail::Disabler<ARGS...>::disable(_args...);
    }

    template<typename F>
    void enabled(GLuint _switch, F f)
    {
      detail::Enabler<GLuint>::enable(_switch);
      f();
      detail::Disabler<GLuint>::disable(_switch);
    }

    template<typename F>
    void disabled(GLuint _switch, F f)
    {
      detail::Disabler<GLuint>::disable(_switch);
      f();
      detail::Enabler<GLuint>::enable(_switch);
    }
  }
}
