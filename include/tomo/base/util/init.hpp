#pragma once

namespace tomo
{
  namespace base
  {
    namespace util
    {
      namespace functor
      {
        template<typename T>
        struct Init
        {
          void operator()(T& _t) {}
        };

        template<typename T>
        struct Init<T*>
        {
          template<typename PTR>
          void operator()(PTR& _t)
          {
            _t = nullptr;
          }
        };

        template<typename T>
        struct Init<T const*> : Init<T*> {};
      }

      template<typename T>
      void init(T& _t)
      {
        functor::Init<T>()(_t);
      }
    }
  }
}
