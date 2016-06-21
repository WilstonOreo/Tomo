#pragma once

#include <gex/base.hpp>
#include <gex/comp.hpp>
#include <gex/io.hpp>
#include "Item.hpp"

namespace tomo
{
  namespace item
  {
    struct PointCloudLoader
    {
      template<typename PATH, typename ITEM>
      bool operator()(const PATH& _path, ITEM& _item)
      { 
        TOMO_DBG("Loading " << _path);
        return gex::io::read(_path,_item.data_);
      }
    };

    class PointCloud : public Item<PointCloud,PointCloudLoader> 
    {
      TBD_PARAMETER_LIST
      (
        (float) rotate_x,
        (float) rotate_y,
        (float) rotate_z,
        (gex::Vec3) offset,
        (gex::Vec3) scale
      )
    public:
      friend PointCloudLoader;
      typedef gex::PointCloud::bounds_type bounds_type;
      TOMO_ITEM("generic",PointCloud)

      PointCloud(parent_type const* _parent) : 
        Item<PointCloud,PointCloudLoader>(_parent),
        rotate_x_(0),
        rotate_y_(0),
        rotate_z_(0),
        offset_(0,0,0),
        scale_(1,1,1) {}

      inline bool loadFromFile(const std::string& _filename)
      {
        return gex::io::read(_filename,data_);
      }

      inline bounds_type transformedBounds() const
      {
        typedef gex::PointCloud::point_type point_type;
        bounds_type _bounds;
        auto&& _transform = transformation();
        gex::for_each<point_type>(data(),[&_bounds,&_transform](const point_type& _p)
        {
          _bounds.extend(_transform * _p);
        });
        return _bounds;
      }

      inline gex::Transform3 transformation() const
      {
        auto&& _center = data().bounds().center();
        return gex::base::transformation(
            _center,offset_,
            rotate_x(),rotate_y(),rotate_z(),
            scale());
      }

      gex::Vec3& offset()
      {
        return offset_;
      }

      TBD_PROPERTY_REF_RO(gex::PointCloud,data)
    };
  }
}

