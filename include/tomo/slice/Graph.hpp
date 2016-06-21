#pragma once

#include <list>
#include <tbd/property.h>
#include <gex/index.hpp>
#include "tomo/base/assert.hpp"
#include "Set.hpp"
#include <gex/algorithm/intersects.hpp>

namespace tomo
{
  namespace slice
  {
    template<typename SLICE>
    struct Graph
    {
      typedef SLICE slice_type;
      typedef Graph<slice_type> type;
      typedef typename slice_type::bounds_type bounds_type;
      typedef std::vector<slice_type> slice_container_type;
      typedef SetMutable<slice_type> sliceset_mutable_type;
      typedef SetConst<slice_type> sliceset_const_type;
      typedef typename bounds_type::scalar_type scalar_type;
      typedef std::map<scalar_type,gex::MultiLineString> defectslices_map_type;
      typedef int slice_index_type;

      Graph() 
      {
        clear();
      }

      void clear()
      {
        minSliceHeight_ = gex::inf();
        maxSliceHeight_ = 0.0;
        averageSliceHeight_ = 0.0;
        bounds_ = bounds_type();
        slices_.clear();
        sliceTree_.clear();
        defectSlices_.clear();
      }

      bool empty() const
      {
        return slices_.empty();
      }

      void insertDefect(const scalar_type& _s, const gex::MultiLineString& _lineStrings)
      {
        defectSlices_.insert({_s,_lineStrings});
      }

      void insert(slice_type&& _slice)
      {
        auto& _inserted = insertSlice(_slice);
        sliceTree_.insert(sliceToNode(_inserted,slices_.size()-1)); 
      }
     
      template<typename INDEXES>
      sliceset_const_type indexesToPointers(const INDEXES& _indexes) const
      {
        sliceset_const_type _slices;
        for (auto _index :_indexes)
        {
          _slices.insert(&slices_[_index]);
        }
        return _slices;
      }
 
      void insert(const slice_type& _slice)
      {
        auto& _inserted = insertSlice(_slice);
        sliceTree_.insert(sliceToNode(_inserted,slices_.size()-1));
      }

      void insert(const slice_container_type& _slices)
      {
        std::vector<slice_node_type> _values;
        _values.reserve(_slices.size());
        for (auto& _slice : _slices)
        {
          auto& _inserted = insertSlice(_slice); 
          if (sliceIsValid(_slice))
          _values.push_back(sliceToNode(_inserted),slices_.size()-1);
        }
        sliceTree_.insert(_values.begin(),_values.end());
      }

      std::vector<SLICE const*> fetch() const 
      {
        std::vector<SLICE const*> _slices;
        std::for_each(slices_.begin(),slices_.end(),[&](const SLICE& _slice)
        {
          _slices.push_back(&_slice);
        });
        return _slices;
      }

      template<typename FUNCTOR>
      void change(const slice_type& _slice, FUNCTOR f)
      {
        auto&& _value = std::make_pair(_slice.bounds(), &_slice);
        sliceTree_.remove(_value);
        auto _newSlice = _slice;
        f(_newSlice);
        insertSlice(_newSlice);
      }

      template<typename PREDICATE>
      sliceset_const_type query(const PREDICATE& _pred) const
      {
        std::vector<slice_node_type> _found;
        gex::index::query(sliceTree_,_pred,std::back_inserter(_found));
        sliceset_const_type _slices;
        for (auto& _v : _found)
        {
          _slices.insert(&slices_[_v.second]);
        }
        return _slices;
      }
      
      template<typename PREDICATE>
      std::set<slice_index_type> queryIndexes(const PREDICATE& _pred) const
      {
        std::vector<slice_node_type> _found;
        gex::index::query(sliceTree_,_pred,std::back_inserter(_found));
        std::set<slice_index_type> _slices;
        for (auto& _v : _found)
        {
          _slices.insert(_v.second);
        }
        return _slices;
      }


      void connect()
      {
        for (auto& _slice : slices_)
          connect(_slice);
      }

      TBD_PROPERTY_REF_RO(bounds_type,bounds)
      TBD_PROPERTY_REF_RO(slice_container_type,slices)
      TBD_PROPERTY_REF_RO(defectslices_map_type,defectSlices)

      TBD_PROPERTY_REF_RO(scalar_type,minSliceHeight)
      TBD_PROPERTY_REF_RO(scalar_type,maxSliceHeight)
      TBD_PROPERTY_REF_RO(scalar_type,averageSliceHeight)
    private:
      typedef gex::RTree3Node<slice_index_type> slice_node_type;
      typedef typename slice_type::range_type range_type;

      slice_type const& insertSlice(const slice_type& _slice)
      {
        bounds_.extend(_slice.bounds());
        
        auto _sliceHeight = _slice.range().length();
        if (_sliceHeight < minSliceHeight_) minSliceHeight_ = _sliceHeight;
        if (_sliceHeight > maxSliceHeight_) maxSliceHeight_ = _sliceHeight;
        averageSliceHeight_ = (averageSliceHeight_ * slices_.size() + _sliceHeight ) / (slices_.size() + 1);

        slices_.push_back(_slice); 
        return slices_.back();
      }

      enum Location { SAME, LOWER, UPPER };

      static bool sliceIsValid(const slice_type& _slice)
      {
        return !_slice.polygon().boundary().empty();
      }

      /// Check if the slice ranges
      Location location(
          const range_type& _a, 
          const range_type& _b)
      {
        auto _middle = _a.middle();
        if (_b.max() < _middle) return LOWER; else
        if (_b.min() > _middle) return UPPER;
        return SAME;
      }

      void connect(slice_type& _slice)
      {
        auto& _b = _slice.polygon().bounds();
        auto& _max = _slice.range().max();
        auto& _min = _slice.range().min();
        auto _l = _slice.range().length() / 2.0;
        auto _bounds = _b.expand(slice_type::axis(),range_type(_min - _l,_max + _l));

        auto&& _sliceIndices = queryIndexes(gex::index::intersects(_bounds));
        for (auto& _index : _sliceIndices)
        {
          auto& _s = slices_[_index];
          Location _location = location(_slice.range(),_s.range());
          if (_location == SAME) continue;
          if (gex::algorithm::intersects(_s.polygon(),_slice.polygon()))
          {
            if (_location == LOWER) 
              _slice.lower().insert(_index);
            else
            if (_location == UPPER)
              _slice.upper().insert(_index);
          }
        }
      }

      slice_node_type sliceToNode(const slice_type& _slice, int _index)
      {
        return slice_node_type(_slice.bounds(),_index);
      }

      gex::RTree3<slice_index_type> sliceTree_; 
    };
  }
}
