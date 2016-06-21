#pragma once

#include "Layer.hpp"
#include "Graph.hpp"

namespace tomo
{
  namespace slice
  {
    namespace strategy
    {
      namespace for_each 
      {
        struct Default {};
      }
    }

    namespace functor
    {
      template<typename SLICES, typename STRATEGY = slice::strategy::for_each::Default>
      struct ForEach
      {
        template<typename FUNCTOR, typename S>
        void operator()(SLICES& _slices, S, FUNCTOR f)
        {
          std::for_each(_slices.begin(),_slices.end(),f);
        }

        template<typename FUNCTOR, typename S>
        void operator()(const SLICES& _slices, S, FUNCTOR f)
        {
          std::for_each(_slices.begin(),_slices.end(),f);
        }
      };
    }

    using functor::ForEach;

    template<typename SLICES, typename FUNCTOR>
    void for_each(const SLICES& _slices, FUNCTOR _f)
    {
      ForEach<SLICES,strategy::for_each::Default>()(_slices,strategy::for_each::Default(),_f);
    }

    template<typename SLICES, typename FUNCTOR>
    void for_each(SLICES& _slices, FUNCTOR _f)
    {
      ForEach<SLICES,strategy::for_each::Default>()(_slices,strategy::for_each::Default(),_f);
    }

    template<typename SLICES, typename STRATEGY, typename FUNCTOR>
    void for_each(const SLICES& _slices, STRATEGY _s, FUNCTOR _f)
    {
      ForEach<SLICES,STRATEGY>()(_slices,_s,_f);
    }

    template<typename SLICES, typename STRATEGY, typename FUNCTOR>
    void for_each(SLICES& _slices, STRATEGY _s, FUNCTOR _f)
    {
      ForEach<SLICES>()(_slices,_s,_f);
    }

    namespace functor
    {
      template<typename SLICE, typename STRATEGY>
      struct ForEach<Layer<SLICE>,STRATEGY>
      {
        template<typename LAYER, typename S, typename FUNCTOR>
        void operator()(const LAYER& _layer, S, FUNCTOR f)
        {
          for (auto& _slice : _layer.slices()) f(*_slice);
        }
      };

      template<typename SLICE, typename STRATEGY>
      struct ForEach<Graph<SLICE>,STRATEGY>
      {
        typedef Graph<SLICE> graph_type;

        template<typename FUNCTOR, typename S>
        void operator()(const graph_type& _graph, S, FUNCTOR f)
        {
          for (auto& _slice : _graph.slices())
          {
            f(_slice);
          }
        }
        
        /*template<typename FUNCTOR>
        void operator()(graph_type& _graph, FUNCTOR f)
        {
          auto _slices = _graph.slices();
          _graph.clear();
          for (auto& _slice : _slices)
          {
            f(_slice);
          }
          _graph.insert(_slices);
          _graph.connect();
        }*/
      };
    }
  }
}

