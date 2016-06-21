#pragma once

#include <tbd/boolean.h>
#include <tbd/add_const_if.h>
#include "Set.hpp"
#include "Graph.hpp"
#include "Layer.hpp"

namespace tomo
{
  namespace slice
  {
    namespace strategy
    {
      using gex::Scalar;
      using gex::Range;

      struct LayerHeightTopBottom
      {
        LayerHeightTopBottom(Scalar _layerHeight) :
          layerHeight_(_layerHeight) {}
        
        TBD_PROPERTY_REF(Scalar,layerHeight)
      };

      struct LayerHeightBottomTop
      {
        LayerHeightBottomTop(Scalar _layerHeight) :
          layerHeight_(_layerHeight) {}

        TBD_PROPERTY_REF(Scalar,layerHeight)
      };
      
      struct LayerHeightRangeTopBottom : LayerHeightTopBottom
      {
        LayerHeightRangeTopBottom(
            const Range& _range, 
            Scalar _layerHeight) :
          LayerHeightTopBottom(_layerHeight),
          range_(_range) {}
        
        TBD_PROPERTY_REF(Range,range)
      };

      struct LayerHeightRangeBottomTop : LayerHeightBottomTop
      {
        LayerHeightRangeBottomTop(
            const Range& _range, 
            Scalar _layerHeight) :
          LayerHeightBottomTop(_layerHeight),
          range_(_range) {}
        
        TBD_PROPERTY_REF(Range,range)
      };
    }

    namespace functor
    {
      namespace detail
      {
        template<typename SET, typename FUNCTOR>
        void remove_if(SET& _set, FUNCTOR _f)
        {
          auto it = _set.begin();
          while (it != _set.end())
          {
            if (_f(*it))
              it = _set.erase(it);
            else
              ++it;
          }
        }
      }

      /// Functor object for traversing each layer
      template<typename SLICE, typename STRATEGY, bool IS_CONST = true>
      struct ForEachLayer {}; 

      template<typename SLICE, bool IS_CONST>
      struct ForEachLayer<SLICE,strategy::LayerHeightRangeBottomTop,IS_CONST>
      {
        /// @todo User-defined Direction
        typedef SLICE slice_type;
        typedef Layer<slice_type> layer_type;
        typedef Graph<slice_type> graph_type;
        typedef typename tbd::AddConstIf<graph_type,IS_CONST>::ref_type graph_ref_type;
        typedef typename SetType<slice_type>::type sliceset_type; 
        typedef typename graph_type::bounds_type bounds_type;
        typedef typename bounds_type::range_type range_type;

      private:
        static constexpr gex::Axis axis() { return slice_type::axis(); }
        
        template<typename LAYERBOUNDS, typename SLICES>
        static void removeUpper(const LAYERBOUNDS& _layerBounds, SLICES& _set)
        {
          detail::remove_if(_set,[&_layerBounds](SLICE const* _slice)
          {
            return _slice->bounds().min()[axis()] >= _layerBounds.max()[axis()];
          });
        }

      public:
        template<typename STRATEGY, typename FUNCTOR>
        void operator()(
            graph_ref_type _graph, 
            STRATEGY _strategy, 
            FUNCTOR f)
        {
          auto& _layerHeight = _strategy.layerHeight();
          auto& _layerRange = _strategy.range();
          TOMO_ASSERT(_layerHeight > 0);
          
          auto _range = _layerRange;
          _range.max(_range.min() + _layerHeight);        
          sliceset_type _lastLayerSlices; 

          bounds_type _layerBounds = 
            _graph.bounds().project(axis()).expand(axis(),_range);

          while (_range.min() < _layerRange.max())
          {
            using tbd::boolean::operator-;
            using tbd::boolean::operator|=;
            auto&& _layerSlices = _graph.query(gex::index::intersects(_layerBounds))- _lastLayerSlices;
            removeUpper(_layerBounds,_layerSlices);
            f(layer_type(_layerBounds,_layerSlices));
            _lastLayerSlices |= _layerSlices;
            if (_lastLayerSlices.size() >= _graph.slices().size()) break;
            _range += _layerHeight;
            _layerBounds.range(slice_type::axis(),_range);
          }
        }
      };
      template<typename SLICE, bool IS_CONST>
      struct ForEachLayer<SLICE,strategy::LayerHeightRangeTopBottom,IS_CONST>
      {
        /// @todo User-defined Direction
        typedef SLICE slice_type;
        typedef Layer<slice_type> layer_type;
        typedef Graph<slice_type> graph_type;
        typedef typename tbd::AddConstIf<graph_type,IS_CONST>::ref_type graph_ref_type;
        typedef typename SetType<slice_type>::type sliceset_type; 
        typedef typename graph_type::bounds_type bounds_type;
        typedef typename bounds_type::range_type range_type;

      private:
        static constexpr gex::Axis axis() { return slice_type::axis(); }
        
        template<typename LAYERBOUNDS, typename SLICES>
        static void removeLower(const LAYERBOUNDS& _layerBounds, SLICES& _set)
        {
          detail::remove_if(_set,[&_layerBounds](SLICE const* _slice)
          {
            return _slice->bounds().max()[axis()] <= _layerBounds.min()[axis()];
          });
        }

      public:
        template<typename STRATEGY, typename FUNCTOR>
        void operator()(
            graph_ref_type _graph, 
            STRATEGY _strategy, 
            FUNCTOR f)
        {
          auto& _layerHeight = _strategy.layerHeight();
          auto& _layerRange = _strategy.range();
          TOMO_ASSERT(_layerHeight > 0);
          
          auto _range = _layerRange;
          _range.min(_range.max() - _layerHeight);        
          sliceset_type _lastLayerSlices; 

          bounds_type _layerBounds = 
            _graph.bounds().project(axis()).expand(axis(),_range);

          while (_range.min() > _layerRange.min())
          {
            using tbd::boolean::operator-;
            using tbd::boolean::operator|=;
            auto&& _layerSlices = _graph.query(gex::index::intersects(_layerBounds)) - _lastLayerSlices;
            removeLower(_layerBounds,_layerSlices);
            f(layer_type(_layerBounds,_layerSlices));
            _lastLayerSlices |= _layerSlices;
            if (_lastLayerSlices.size() >= _graph.slices().size()) break;

            _range -= _layerHeight;
            _layerBounds.range(slice_type::axis(),_range);
          }
        }
      };

      template<typename SLICE, bool IS_CONST>
      struct ForEachLayer<SLICE,strategy::LayerHeightBottomTop,IS_CONST> 
      {
        typedef SLICE slice_type;
        typedef Layer<slice_type> layer_type;
        typedef Graph<slice_type> graph_type;
        typedef typename tbd::AddConstIf<graph_type,IS_CONST>::ref_type graph_ref_type;
        
        template<typename STRATEGY, typename FUNCTOR>
        void operator()(
            graph_ref_type _graph, 
            STRATEGY _strategy, 
            FUNCTOR f)
        {
          ForEachLayer<SLICE,strategy::LayerHeightRangeBottomTop,IS_CONST>()(_graph,
              strategy::LayerHeightRangeBottomTop(
                _graph.bounds().range(gex::Z),
                _strategy.layerHeight()),
              f);
        }
      };

      template<typename SLICE, bool IS_CONST>
      struct ForEachLayer<SLICE,strategy::LayerHeightTopBottom,IS_CONST> 
      {
        typedef SLICE slice_type;
        typedef Layer<slice_type> layer_type;
        typedef Graph<slice_type> graph_type;
        typedef typename tbd::AddConstIf<graph_type,IS_CONST>::ref_type graph_ref_type;
        
        template<typename STRATEGY, typename FUNCTOR>
        void operator()(
            graph_ref_type _graph, 
            STRATEGY _strategy, 
            FUNCTOR f)
        {
          ForEachLayer<SLICE,strategy::LayerHeightRangeTopBottom,IS_CONST>()(_graph,
              strategy::LayerHeightRangeTopBottom(
                _graph.bounds().range(gex::Z),
                _strategy.layerHeight()),
              f);
        }
      };
    }

    using functor::ForEachLayer;

    template<typename SLICE, typename STRATEGY, typename FUNCTOR>
    void for_each_layer(const Graph<SLICE>& _layers, STRATEGY _strategy, FUNCTOR _f)
    {
      ForEachLayer<SLICE,STRATEGY,true>()(_layers,_strategy,_f);
    }
    
    template<typename SLICE, typename FUNCTOR>
    void for_each_layer(const Graph<SLICE>& _layers, FUNCTOR _f)
    {
      typedef strategy::LayerHeightBottomTop strategy_type;

      ForEachLayer<SLICE,strategy_type,true>()
        (_layers,strategy_type(_layers.minSliceHeight()),_f);
    }
  }
}
