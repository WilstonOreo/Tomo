#include <tomo/tool/Tool.hpp>
#include <tomo/item/Sink.hpp>
#include <tomo/item/Mesh.hpp>
#include <tomo/item/PointCloud.hpp>

namespace tomo
{
  using namespace gex;

  class Place :
    public Tool,
    public Serializer<Place>
  {
    TBD_PARAMETER_LIST
    (
      (MultiSink<item::Mesh,item::PointCloud>) objects,
      (Bounds3) bounds,
      (Scalar) distance,
      (Bool) z_drop
    )
    public:
      typedef Serializer<Place> serializer_type;
      typedef Tool base_type;
      TOMO_TOOL(Place)
      
      Place(mutable_parent_type _parent) :
        base_type(_parent),
        serializer_type(*this),
        objects_(_parent),
        bounds_(Vec3(0,0,0),Vec3(100,100,100)),
        distance_(10.0),
        z_drop_(true) {}

      void process()
      {
        typedef TriangleMesh::bounds_type bounds_type; 

        for (auto& _obj : objects_)
        {
          if (_obj.is<item::Mesh>())
          {
            auto& _meshItem = *_obj.as<item::Mesh>(); 
            auto _bounds = _meshItem.transformedBounds();
          
            if (z_drop())
              _meshItem.offset()(Z) -= _bounds.min().z() + bounds_.min().z();
          } else
          if (_obj.is<item::PointCloud>())
          {
            auto& _pcItem = *_obj.as<item::PointCloud>(); 
            auto _bounds = _pcItem.transformedBounds();

            std::cout << _bounds << std::endl;

            if (z_drop())
              _pcItem.offset()(Z) -= _bounds.min().z() + bounds_.min().z();
          }
        }
      }
  };
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::Place)

