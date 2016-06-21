#pragma once 

#include "SourceHandler.hpp"
#include "SinkHandler.hpp"

#include "getTypeId.hpp"

namespace tomo
{
  namespace item
  {
    namespace detail
    {
      template<typename...ARGS>
      struct SameType {};

      template<typename ARG>
      struct SameType<ARG>
      {
        bool operator()(const ARG& _arg)
        {
          return true;
        }
      };

      template<typename ARG1, typename ARG2>
      struct SameType<ARG1,ARG2>
      {
        bool operator()(const ARG1& _arg1, const ARG2& _arg2)
        {
          if (!_arg1.valid() || !_arg2.valid()) return true;
          return getTypeId(_arg1) == getTypeId(_arg2);
        }
      };

      template<typename ARG1, typename ARG2, typename...ARGS>
      struct SameType<ARG1,ARG2,ARGS...> : 
        private SameType<ARG1,ARG2>,
        private SameType<ARG2,ARGS...>
      {
        typedef SameType<ARG1,ARG2> head_type;
        typedef SameType<ARG2,ARGS...> tail_type;

        bool operator()(const ARG1& _arg1, const ARG2& _arg2, const ARGS&..._args)
        {
          return head_type::operator()(_arg1,_arg2) && 
                 tail_type::operator()(_arg2,_args...);
        }
      };


      template<typename...ARGS>
      bool same_type(const ARGS&..._args)
      {
        return SameType<ARGS...>()(_args...);
      }
    }

    using detail::same_type;
  }
}
