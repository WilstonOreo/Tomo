#pragma once

#include <tomo/base/types/time.hpp>
#include <gex/algorithm/distance.hpp>
#include <gex/algorithm/for_each.hpp>

#include "for_each_path.hpp"

namespace tomo
{
  namespace routing
  {
    namespace functor
    {
      template<typename PATHS>
      struct Duration
      {
        typedef double duration_type;

        void operator()(const PATHS& _paths, duration_type& _duration)
        {
          typedef typename ForEachPath<PATHS,true>::path_type path_type;
          typedef typename path_type::point_type move_type;
          for_each_path(_paths,[&](const path_type& _path)
          {
            typedef std::pair<move_type,move_type> move_pair;
            gex::for_each<move_pair>(_path,[&_duration](const move_type& _prev, const move_type& _next)
            {
              auto _length = gex::distance(_prev,_next);
              _duration += _length / _prev.speed();
            });
          });
        }
      };
    }

    template<typename T, typename DURATION>
    void duration(const T& _t, DURATION& _duration)
    {
      functor::Duration<T>()(_t,_duration);
    }

    template<typename T>
    typename functor::Duration<T>::duration_type duration(const T& _t)
    {
      typename functor::Duration<T>::duration_type _duration = 0.0;
      duration(_t,_duration);
      return _duration;
    }
  }
}
