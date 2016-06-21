#pragma once

#include "for_each_path.hpp"

namespace tomo
{
  namespace routing
  {
    namespace functor
    {
      template<typename PATHS>
      struct NumPaths 
      {
        typedef size_t num_paths_type;
      
        void operator()(const PATHS& _paths, num_paths_type& _num_paths)
        {
          typedef typename ForEachPath<PATHS,true>::path_type path_type;
          typedef typename path_type::point_type move_type;
          _num_paths = 0;
          for_each_path(_paths,[&](const path_type& _path)
          {
            ++_num_paths;
          });
        }
      };
    }

    template<typename T, typename NUMPATHS>
    void num_paths(const T& _t, NUMPATHS& _num_paths)
    {
      functor::NumPaths<T>()(_t,_num_paths);
    }

    template<typename T>
    typename functor::NumPaths<T>::num_paths_type num_paths(const T& _t)
    {
      typename functor::NumPaths<T>::num_paths_type _num_paths = 0;
      num_paths(_t,_num_paths);
      return _num_paths;
    }
  }
}

