#pragma once

#include <tomo/base/gex.hpp>
#include <gex/comp.hpp>
#include <gex/io.hpp>
#include "Item.hpp"

namespace tomo
{
  namespace item
  {
    struct MeshLoader
    {
      template<typename PATH, typename ITEM>
      bool operator()(const PATH& _path, ITEM& _item)
      { 
        TOMO_DBG("Loading " << _path);
        return gex::io::read(_path,_item.data_);
      }
    };

    typedef gex::Vec3 Rotation;

    class Mesh : public Item<Mesh,MeshLoader>
    {
      TBD_PARAMETER_LIST
      (
        (Rotation) rotate,
        (Vec3) offset,
        (Vec3) scale
      )
    public:
      friend MeshLoader;
      typedef gex::TriangleMesh::bounds_type bounds_type;
      TOMO_ITEM("generic",Mesh)

      Mesh(parent_type const* _parent) : 
        Item<Mesh,MeshLoader>(_parent),
        rotate_(0,0,0),
        offset_(0,0,0),
        scale_(1,1,1) {}

      inline bool loadFromFile(const std::string& _filename)
      {
        return gex::io::read(_filename,data_);
      }

      inline bounds_type transformedBounds() const
      {
        typedef gex::TriangleMesh::point_type point_type;
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
            rotate().x(),rotate().y(),rotate().z(),
            scale());
      }

      gex::Vec3& offset()
      {
        return offset_;
      }

      TBD_PROPERTY_REF_RO(gex::TriangleMesh,data)
    };
    

  }
}

