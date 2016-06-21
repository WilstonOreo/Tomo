#pragma once

#include "Path.hpp"
#include "Route.hpp"
#include <tomo/slice/for_each.hpp>
#include <tomo/slice/Layer.hpp>

namespace tomo
{
  namespace routing
  {
    namespace functor
    {
      template<typename SLICE, bool IS_CONST>
      struct ForEachPath 
      {
        typedef SLICE slice_type;
        typedef typename tbd::AddConstIf<slice_type,IS_CONST>::ref_type ref_type;

        template<typename FUNCTOR>
        void operator()(ref_type _slice, FUNCTOR f)
        {
          for (auto& _path : _slice.paths())
            f(_path);
        }
      };

      template<typename MOVE, bool IS_CONST>
      struct ForEachPath<Path<MOVE>,IS_CONST>
      {
        typedef Path<MOVE> path_type;
        typedef typename tbd::AddConstIf<path_type,IS_CONST>::ref_type ref_type;

        template<typename FUNCTOR>
        void operator()(ref_type _path, FUNCTOR f)
        {
          f(_path);
        }
      };


      template<typename SLICE, bool IS_CONST>
      struct ForEachPath<slice::Layer<SLICE>,IS_CONST>
      {
        typedef typename SLICE::path_type path_type;
        
        template<typename T, typename FUNCTOR>
        void operator()(const T& _layer, FUNCTOR f)
        {
          typedef typename T::slice_type slice_type;

          tomo::slice::for_each(_layer,[&](const SLICE& _slice)
          {
            ForEachPath<SLICE,true>()(_slice,f);
          });
        }
      };

      template<typename SLICE, bool IS_CONST>
      struct ForEachPath<slice::Graph<SLICE>,IS_CONST>
      {
        typedef typename SLICE::path_type path_type;
        
        template<typename T, typename FUNCTOR>
        void operator()(const T& _graph, FUNCTOR f)
        {
          typedef typename T::slice_type slice_type;
          tomo::slice::for_each(_graph,[&](const SLICE& _slice)
          {
            ForEachPath<SLICE,true>()(_slice,f);
          });
        }
      };

      template<typename PATH, typename STATE, bool IS_CONST>
      struct ForEachPath<Route<PATH,STATE>,IS_CONST>
      {
        typedef PATH path_type;
        typedef Route<path_type,STATE> route_type;
        typedef typename tbd::AddConstIf<route_type,IS_CONST>::ref_type ref_type;
      
        template<typename FUNCTOR>
        void operator()(ref_type _route, FUNCTOR f)
        {
          f(_route.path());
        }
      };
    }

    using functor::ForEachPath;

    template<typename PATHS, typename FUNCTOR>
    void for_each_path(const PATHS& _paths, FUNCTOR f)
    {
      ForEachPath<PATHS,true>()(_paths,f);
    }
    
    template<typename PATHS, typename FUNCTOR>
    void for_each_path(PATHS& _paths, FUNCTOR f)
    {
      ForEachPath<PATHS,false>()(_paths,f);
    }
  }
}
