#pragma once

#include <gex/algorithm/perimeter.hpp>
#include "for_each_path.hpp"

namespace tomo
{
  namespace routing
  {
    namespace functor
    {
      template<typename PATHS>
      struct Length
      {
        typedef double length_type;
      
        void operator()(const PATHS& _paths, length_type& _length)
        {
          typedef typename ForEachPath<PATHS,true>::path_type path_type;
          _length = 0.0;
          for_each_path(_paths,[&](const path_type& _path)
          {
            _length += gex::perimeter(_path);
          });
        }      
      };
    }

    template<typename T, typename LENGTH>
    void length(const T& _t, LENGTH& _length)
    {
      functor::Length<T>()(_t,_length);
    }

    template<typename T>
    typename functor::Length<T>::length_type length(const T& _t)
    {
      typename functor::Length<T>::length_type _length;
      length(_t,_length);
      return _length;
    }
  }
}

