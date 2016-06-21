#pragma once

#include <gex/algorithm/for_each.hpp>
#include "for_each_path.hpp"

namespace tomo
{
  namespace routing
  {
    namespace functor
    {
      template<typename PATHS>
      struct ForEachMove
      {
        typedef typename ForEachPath<PATHS,true>::path_type path_type;
        typedef typename path_type::point_type move_type;

        template<typename FUNCTOR>
        void operator()(const PATHS& _paths, FUNCTOR f)
        {
          for_each_path(_paths,[&](const path_type& _path)
          {
            gex::for_each<move_type>(_path,[&](const move_type& _move) 
              { f(_move); });
          });
        }
        
        template<typename FUNCTOR>
        void operator()(PATHS& _paths, FUNCTOR f)
        {
          for_each_path(_paths,[&](path_type& _path)
          {
            gex::for_each<move_type>(_path,[&](move_type& _move) 
              { f(_move); });
          });
        }
      };
    }

    using functor::ForEachMove;

    template<typename PATHS, typename FUNCTOR>
    void for_each_move(const PATHS& _paths, FUNCTOR _f)
    {
      ForEachMove<PATHS>()(_paths,_f);
    }
    
    template<typename PATHS, typename FUNCTOR>
    void for_each_move(PATHS& _paths, FUNCTOR _f)
    {
      ForEachMove<PATHS>()(_paths,_f);
    }
  }
}
