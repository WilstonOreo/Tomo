#pragma once

namespace tomo
{
  namespace routing
  {
    template<typename PATHS, typename FUNCTOR> 
    void for_each_move(const PATHS& _paths, FUNCTOR _f);

    namespace functor
    {
      template<typename PATHS> struct ForEachMove;

      template<typename PATHS>
      struct Bounds
      {
        template<typename BOUNDS>
        void operator()(const PATHS& _paths, BOUNDS& _bounds)
        {
          typedef typename ForEachMove<PATHS>::move_type move_type;
          typedef typename move_type::pos_type pos_type;

          for_each_move(_paths,[&](const move_type& _move)
          {
            _bounds.extend(pos_type(_move));
          });
        }
      };
    }

    template<typename PATHS, typename BOUNDS>
    void bounds(const PATHS& _paths, BOUNDS& _bounds)
    {
      functor::Bounds<PATHS>()(_paths,_bounds);
    }

    template<typename PATHS>
    gex::Bounds3 bounds(const PATHS& _paths)
    {
      gex::Bounds3 _bounds;
      bounds(_paths,_bounds);
      return _bounds;
    }
  }
}
