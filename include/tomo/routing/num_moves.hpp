#pragma once

#include <gex/algorithm/num.hpp>
#include "for_each_path.hpp"

namespace tomo
{
  namespace routing
  {
    namespace functor
    {
      template<typename PATHS>
      struct NumMoves 
      {
        typedef size_t num_moves_type;
      
        void operator()(const PATHS& _paths, num_moves_type& _num_moves)
        {
          typedef typename ForEachPath<PATHS,true>::path_type path_type;
          typedef typename path_type::point_type move_type;
          _num_moves = 0;
          for_each_path(_paths,[&](const path_type& _path)
          {
            _num_moves += gex::num<move_type>(_path);
          });
        }
      };
    }

    template<typename T, typename NUMMOVES>
    void num_moves(const T& _t, NUMMOVES& _num_moves)
    {
      functor::NumMoves<T>()(_t,_num_moves);
    }

    template<typename T>
    typename functor::NumMoves<T>::num_moves_type num_moves(const T& _t)
    {
      typename functor::NumMoves<T>::num_moves_type _num_moves = 0;
      num_moves(_t,_num_moves);
      return _num_moves;
    }
  }
}

