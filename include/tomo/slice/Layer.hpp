#pragma once

#include <tbd/property.h>
#include <tbd/add_const_if.h>
#include "Set.hpp"
#include "List.hpp"
#include "Graph.hpp"

namespace tomo
{
  namespace slice
  {
    template<typename SLICE>
    struct Layer
    {
      static constexpr gex::Axis axis() { return slice_type::axis(); }

      typedef SLICE slice_type;
      typedef Graph<SLICE> graph_type;
      typedef typename SetType<slice_type,true>::type sliceset_const_type;
      typedef typename ListType<slice_type,true>::type slicelist_const_type;
      typedef typename ListType<slice_type,false>::type slicelist_mutable_type;

      typedef typename slice_type::bounds_type bounds_type;
      typedef typename bounds_type::range_type range_type;

      Layer(const range_type& _range, const graph_type& _graph)
      {
        bounds_ = _graph.bounds().project(axis()).expand(axis(),_range);
        slices_ = _graph.querySlices(gex::index::intersects(bounds_));
      }

      Layer(const bounds_type& _bounds, 
          const sliceset_const_type& _slices) :
        bounds_(_bounds),
        slices_(_slices)
      {}
      
      slicelist_mutable_type fetch()
      {
        slicelist_mutable_type _sliceList;
        _sliceList.reserve(slices_.size());
        for (auto _slice : slices_)
          _sliceList.push_back(_slice);
        return _sliceList;
      }

      slicelist_const_type fetch() const
      {
        slicelist_const_type _sliceList;
        _sliceList.reserve(slices_.size());
        for (auto _slice : slices_)
          _sliceList.push_back(_slice);
        return _sliceList;
      }

      template<typename FUNCTOR>
      slicelist_mutable_type fetch(FUNCTOR f) 
      {
        slicelist_mutable_type _sliceList = fetch();
        std::sort(_sliceList.begin(),_sliceList.end(),f);
        return _sliceList;
      }

      template<typename FUNCTOR>
      slicelist_const_type fetch(FUNCTOR f) const 
      {
        slicelist_const_type _sliceList = fetch();
        std::sort(_sliceList.begin(),_sliceList.end(),f);
        return _sliceList;
      }

      TBD_PROPERTY_REF_RO(bounds_type,bounds)
      TBD_PROPERTY_REF_RO(sliceset_const_type,slices)
    };
  }
}
