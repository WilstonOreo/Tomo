#pragma once

namespace tomo
{
  namespace base
  {
    namespace util
    {
      namespace functor
      {
        /// Functor for iterating over each parent of a tool
        template<typename TOOL>
        struct ForEachParent
        {
          template<typename FUNCTOR>
          void operator()(TOOL& _tool, FUNCTOR f)
          {
            auto _p = _tool.parent();
            while (_p)
            {
              f(_p);
              _p = _p->parent();
            }
          }

          template<typename FUNCTOR>
          void operator()(const TOOL& _tool, FUNCTOR f)
          {
            auto _p = _tool.parent();
            while (_p)
            {
              f(_p);
              _p = _p->parent();
            }
          }
        };

        template<typename T>
        struct ForEach {};

        template<typename T>
        struct ForEach<std::set<T const*>>
        {
          template<typename SET, typename FUNCTOR>
          void operator()(const SET& _set, FUNCTOR f)
          {
            for (auto& _s : _set)
            {
              f(*_s);
            }
          }

          template<typename SET, typename FUNCTOR>
          void operator()(SET& _set, FUNCTOR f)
          {
            for (auto& _s : _set)
            {
              f(*_s);
            }
          }
        };

        template<typename T>
        struct ForEach<std::set<T*>> :
                                      ForEach<std::set<T const*>> {};
      }

      template<typename SET, typename FUNCTOR>
      void for_each(const SET& _set, FUNCTOR f)
      {
        functor::ForEach<SET>()(_set,f);
      }

      template<typename SET, typename FUNCTOR>
      void for_each(SET& _set, FUNCTOR f)
      {
        functor::ForEach<SET>()(_set,f);
      }

      /// Function for iterating over each parent (mutable version)
      template<typename TOOL, typename FUNCTOR>
      void for_each_parent(const TOOL& _tool, FUNCTOR f)
      {
        functor::ForEachParent<TOOL>()(_tool,f);
      }

      /// Function for iterating over each parent (const version)
      template<typename TOOL, typename FUNCTOR>
      void for_each_parent(TOOL& _tool, FUNCTOR f)
      {
        functor::ForEachParent<TOOL>()(_tool,f);
      }
    }
  }
}
