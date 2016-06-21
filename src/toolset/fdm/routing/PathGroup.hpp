#pragma once

#include "../tool/Set.hpp"


namespace tomo
{
  namespace routing
  {
    template<typename PATH, typename TAG = int>
    struct PathGroup
    {
      typedef PATH path_type;
      typedef TAG tag_type;
      typedef path_type const* path_ptr_type;
      typedef gex::Polygon polygon_type;
      typedef gex::Bounds3 bounds_type;

      PathGroup() {}
      PathGroup(const tag_type& _tag) : tag_(_tag) {}
      PathGroup(const tag_type& _tag, const gex::Polygon& _polygon) :
        tag_(_tag),
        polygon_(_polygon) {}

      void clear()
      {
        bounds_ = bounds_type();
        paths_.clear();
      }

      template<typename JOIN_STRATEGY>
      path_type connect(JOIN_STRATEGY _s) const
      {
        path_type _result;
        for (auto& _path : paths_)
          gex::connect(_result,*_path,_s);
        return _result;
      }

      void insert(const path_type& _path)
      {
        auto&& _bounds = routing::bounds(_path);
        bounds_.extend(_bounds);
        paths_.push_back(_path);
      }

      TBD_PROPERTY_REF_RO(std::vector<path_type>,paths)
      TBD_PROPERTY_REF(tag_type,tag)
      TBD_PROPERTY_REF(polygon_type,polygon)
      TBD_PROPERTY_REF_RO(bounds_type,bounds)
    };
  }
}
