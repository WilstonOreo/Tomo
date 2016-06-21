#pragma once

#include "Path.hpp"
#include "Route.hpp"

#include <gex/algorithm/for_each.hpp>
#include "num_moves.hpp"

namespace tomo
{
  namespace routing
  {
    namespace functor
    {
      template<typename PATHS>
      struct AverageSpeed 
      {
        typedef double average_speed_type;
      
        void operator()(const PATHS& _paths, average_speed_type& _average_speed)
        {
          typedef typename ForEachPath<PATHS,true>::path_type path_type;
          typedef typename path_type::point_type move_type;
          _average_speed = 0;
          auto _numMoves = num_moves(_paths);
          if (_numMoves == 0) return;

          for_each_path(_paths,[&](const path_type& _path)
          {
            gex::for_each<move_type>(_path,[&_average_speed](const move_type& _move)
            {
              _average_speed += _move.speed();
            });
          });
          _average_speed /= _numMoves;
        }      
      };
    }

    template<typename T, typename NUMMOVES>
    void average_speed(const T& _t, NUMMOVES& _average_speed)
    {
      functor::AverageSpeed<T>()(_t,_average_speed);
    }

    template<typename T>
    typename functor::AverageSpeed<T>::average_speed_type average_speed(const T& _t)
    {
      typename functor::AverageSpeed<T>::average_speed_type _average_speed = 0.0;
      average_speed(_t,_average_speed);
      return _average_speed;
    }
  }
}


