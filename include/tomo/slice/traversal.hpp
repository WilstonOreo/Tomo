#pragma once

#include <tbd/boolean.h>

namespace tomo
{
  namespace slice
  {
    namespace strategy
    {
      struct Top 
      {
        typedef Top traversal_type;
      };

      struct Bottom 
      {
        typedef Bottom traversal_type;
      };

      template<typename STRATEGY>
      struct GetSliceSet {};

      template<>
      struct GetSliceSet<strategy::Top>
      {
        template<typename SLICEGRAPH, typename SLICE>
        typename SLICEGRAPH::sliceset_const_type operator()(const SLICEGRAPH& _graph, const SLICE& _slice)
        {
          return _graph.indexesToPointers(_slice.upper());
        }
      };

      template<>
      struct GetSliceSet<strategy::Bottom>
      {
        template<typename SLICEGRAPH, typename SLICE>
        typename SLICEGRAPH::sliceset_const_type operator()(const SLICEGRAPH& _graph, const SLICE& _slice)
        {
          return _graph.indexesToPointers(_slice.lower());
        }
      };

      template<typename STRATEGY, typename SLICEGRAPH ,typename SLICE> 
      typename SLICEGRAPH::sliceset_const_type getSliceSet(const SLICEGRAPH& _graph, const SLICE& _slice)
      {
        return GetSliceSet<STRATEGY>()(_graph,_slice);
      }

      template<typename STRATEGY>
      struct RangeIntersects
      {
      };

      template<>
      struct RangeIntersects<strategy::Top>
      {
        template<typename SLICE, typename OFFSET>
        bool operator()(const SLICE& _a, OFFSET _offset, const SLICE& _b)
        {
          return gex::intersects(_a.range().max() + _offset,_b.range());
        }
      };

      template<>
      struct RangeIntersects<strategy::Bottom>
      {
        template<typename SLICE, typename OFFSET>
        bool operator()(const SLICE& _a, OFFSET _offset, const SLICE& _b)
        {
          return gex::intersects(_a.range().min() - _offset,_b.range());
        }
      };

      template<typename STRATEGY, typename SLICE, typename OFFSET>
      bool rangeIntersects(const SLICE& _a, OFFSET _offset, const SLICE& _b)
      {
        return RangeIntersects<STRATEGY>()(_a,_offset,_b);
      }
    }

    namespace functor
    {
      template<typename SLICE, typename STRATEGY>
      struct Traversal 
      {
        template<typename SLICEGRAPH, typename FUNCTOR>
        void operator()(const SLICEGRAPH& _graph, const SLICE& _slice, STRATEGY, FUNCTOR f)
        {
          typedef typename SLICEGRAPH::sliceset_const_type sliceset_type; 
          sliceset_type&& _upperSlices = strategy::getSliceSet<STRATEGY>(_graph,_slice);
          using tbd::boolean::operator|=;
          using tbd::boolean::operator-;
          sliceset_type&& _done({&_slice});

          while (!_upperSlices.empty())
          {
            f(_upperSlices);
            sliceset_type _buffer;
            for (auto& _upper : _upperSlices)
            {
              _buffer |= strategy::getSliceSet<STRATEGY>(_graph,*_upper);
            }
            _done |= _upperSlices;
            _upperSlices = _buffer - _done; 
          }
        }
      };
    }

    using functor::Traversal;

    template<typename SLICEGRAPH, typename SLICE, typename STRATEGY, typename FUNCTOR>
    void traversal(const SLICEGRAPH& _sliceGraph, const SLICE& _slice, STRATEGY _strategy, FUNCTOR _f)
    {
      Traversal<SLICE,STRATEGY>()(_sliceGraph,_slice,_strategy,_f);
    }
  }
}
