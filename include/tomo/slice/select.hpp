#pragma once

#include <tbd/boolean.h>
#include "for_each.hpp"
#include "for_each_layer.hpp"
#include "util.hpp"

namespace tomo
{
  namespace slice
  {
    namespace functor
    {
      /**@brief Select layers using a functor
        *@detail Selected layer slices are inserted into slicegraph
       **/
      template<typename IN, typename OUT, bool IS_CONST = true>
      struct SelectLayers
      {
        typedef typename tbd::AddConstIf<IN,true>::ref_type in_ref_type; 
        template<typename LAYERHEIGHT, typename FUNCTOR>
        void operator()(IN _in, const LAYERHEIGHT& _layerHeight, OUT& _out, FUNCTOR f)
        {
          typedef typename IN::slice_type slice_type;
          typedef typename slice_type::bounds_type bounds_type;
          typedef typename SetType<slice_type,true>::type sliceset_type;
          _out.clear();
          for_each_layer(_in,_layerHeight,[&](const Layer<slice_type>& _layer) 
          {
            if (f(_layer))
            {
              for (auto& _slice : _layer.slices())
                _out.insert(*_slice);
            }
          });
        }
      };

      /**@brief Select layers using a functor
        *@detail Selected layer slices are inserted into a set of SLICES
       **/
      template<typename SLICE, bool IS_CONST>
      struct SelectLayers<Graph<SLICE>,typename SetType<SLICE,IS_CONST>::type,IS_CONST>
      {
        typedef Graph<SLICE> graph_type;
        typedef SLICE slice_type;
        typedef typename tbd::AddConstIf<graph_type,IS_CONST>::ref_type in_ref_type;
        typedef typename tbd::AddConstIf<Layer<slice_type>,IS_CONST>::ref_type layer_ref_type;
        typedef typename SetType<slice_type,IS_CONST>::type sliceset_type;

        template<typename LAYERHEIGHT, typename FUNCTOR>
        void operator()(in_ref_type _in, const LAYERHEIGHT& _layerHeight, sliceset_type& _out, FUNCTOR f)
        {
          typedef typename slice_type::bounds_type bounds_type;
          _out.clear();
          for_each_layer(_in,_layerHeight,
              [&](layer_ref_type& _layer)
          {
            using tbd::boolean::operator|=;
            if (f(_layer)) _out |= _layer.slices();
          });
        }
      };

    
      /**@brief Select slices using a functor
        *@detail Selected layers are inserted into slicegraph
       **/
      template<typename IN, typename OUT, bool IS_CONST>
      struct Select
      {
        typedef typename tbd::AddConstIf<IN,IS_CONST>::ref_type in_ref_type; 
        
        template<typename FUNCTOR>
        void operator()(in_ref_type _in, OUT& _out, FUNCTOR f)
        {
          typedef typename IN::slice_type slice_type;
          typedef typename tbd::AddConstIf<slice_type,IS_CONST>::ref_type slice_ref_type; 
          _out.clear();
          for_each(_in,[&](slice_ref_type _slice)
          {
            if (f(_slice)) _out.insert(_slice);
          }); 
        }
      };

    /**@brief Select slices using a functor
      *@detail Selected layers are inserted into a set of slices
     **/
      template<typename SLICE, bool IS_CONST>
      struct Select<Graph<SLICE>,typename SetType<SLICE,IS_CONST>::type,IS_CONST>
      {
        typedef SLICE slice_type;
        typedef Graph<SLICE> graph_type;
        typedef typename tbd::AddConstIf<graph_type,IS_CONST>::ref_type in_ref_type; 
        
        template<typename FUNCTOR>
        void operator()(in_ref_type _in, typename SetType<slice_type,IS_CONST>::type& _out, FUNCTOR f)
        {
          typedef typename tbd::AddConstIf<slice_type,IS_CONST>::ref_type slice_ref_type; 
          _out.clear();
          for_each(_in,[&](slice_ref_type _slice)
          {
            if (f(_slice)) _out.insert(&_slice);
          }); 
        }
      }; 
    }

    using functor::Select;
    using functor::SelectLayers;

    template<typename IN, typename LAYERHEIGHT, typename OUT, typename FUNCTOR>
    void select_layers(const IN& _in, const LAYERHEIGHT& _layerHeight, OUT& _out, FUNCTOR f)
    {
      SelectLayers<IN,OUT,true>()(_in,_layerHeight,_out,f);
    }
    
    template<typename IN, typename LAYERHEIGHT, typename OUT, typename FUNCTOR>
    void select_layers(IN& _in, const LAYERHEIGHT& _layerHeight, OUT& _out, FUNCTOR f)
    {
      SelectLayers<IN,OUT,false>()(_in,_layerHeight,_out,f);
    }

    template<typename IN, typename OUT, typename FUNCTOR>
    void select(const IN& _in, OUT& _out, FUNCTOR f)
    {
      Select<IN,OUT,true>()(_in,_out,f);
    }
    
    template<typename IN, typename OUT, typename FUNCTOR>
    void select(IN& _in, OUT& _out, FUNCTOR f)
    {
      Select<IN,OUT,false>()(_in,_out,f);
    }

    template<typename SLICEGRAPH, typename FUNCTOR>
    SLICEGRAPH select(const SLICEGRAPH& _in, FUNCTOR f)
    {
      SLICEGRAPH _out;
      Select<SLICEGRAPH,SLICEGRAPH,false>()(_in,_out,f);
      return _out;
    }
  }
}
