#pragma once

#include "tomo/slice.hpp"
#include "tomo/slice/centroid.hpp"
#include "routing.hpp"

namespace tomo
{
  namespace fdm
  {
    namespace slice
    {
      struct Slice : tomo::slice::Orthogonal<gex::Z>
      {
        typedef tomo::slice::Orthogonal<gex::Z> inherited_type;
        typedef typename inherited_type::tag_type tag_type;
        typedef typename inherited_type::polygon_type polygon_type;
        typedef typename inherited_type::range_type range_type;
        typedef fdm::Path path_type;
        typedef Slice type;
        typedef std::set<int> sliceset_type;

        Slice() : inherited_type(tomo::slice::invalidTag()) {}
        Slice(const range_type& _range, const polygon_type& _polygon,
          const tag_type& _tag = tomo::slice::invalidTag()) :  
          inherited_type(_range,_polygon,_tag) 
        {
          polygon().update();
          boost::geometry::correct(polygon());
        }
     /*
        void swap(Slice& _rhs)
        {
          std::swap(range(),_rhs.range());
          std::swap(polygon(),_rhs.polygon());
          std::swap(pathBounds_,_rhs.pathBounds_);
          std::swap(paths_,_rhs.paths_);
        }

        Slice& operator=(Slice _rhs)
        {
          if (this != &_rhs) swap(_rhs);
          return *this;
        }
*/
        void clear()
        {
          pathBounds_ = bounds_type();
          paths_.clear();
        }

        bool empty() const
        {
          return paths_.empty();
        }        
      void insert(const path_type& _path)
      {
        if (_path.empty()) return;
        auto&& _bounds = routing::bounds(_path);
        auto&& _zRange = _bounds.range(gex::Z);
        _zRange.min() -= height()*0.5;
        _zRange.max() += height()*0.5;
        _bounds.range(gex::Z,_zRange);
        if (!_bounds.valid()) return;
        pathBounds_.extend(_bounds);
        paths_.push_back(_path);
      }

      TBD_PROPERTY_REF_RO(std::vector<path_type>,paths)
      TBD_PROPERTY_REF_RO(bounds_type,pathBounds)
      TBD_PROPERTY_REF(sliceset_type,upper)
      TBD_PROPERTY_REF(sliceset_type,lower)

      };

      typedef tomo::slice::Graph<Slice> Graph;
      typedef tomo::slice::Layer<Slice> Layer;
    }

    using slice::Slice;
    using slice::Layer;
    typedef slice::Graph SliceGraph;
  }

  namespace slice
  {
    namespace functor
    {
      template<>
      struct Centroid<fdm::Slice> : Centroid<slice::Orthogonal<gex::Z>> 
      {
        typedef gex::Point3 output_type;
      };
    }
  }
}
