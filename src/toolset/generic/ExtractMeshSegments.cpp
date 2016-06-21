#include <tomo/tool/Set.hpp>
#include <tomo/tool/Tool.hpp>
#include <tomo/item/Source.hpp>
#include <tomo/slice/sampling.hpp>
#include <tomo/item/Mesh.hpp>

#include <boost/geometry/io/wkt/write.hpp>
#include <fstream>

namespace tomo
{
  namespace tool
  {
    using namespace gex;

    class ExtractMeshSegments :
      public tomo::Tool,
      public Serializer<ExtractMeshSegments>
    {
      TBD_PARAMETER_LIST
      (
        (SingleSource<item::Mesh>) source,
        (String) output_dir,
        (String) prefix,
        (Float) layer_height
      )
    public:
      typedef tomo::Tool base_type;
      typedef Serializer<ExtractMeshSegments> serializer_type;
      TOMO_TOOL(ExtractMeshSegments)

      ExtractMeshSegments(mutable_parent_type _parent) :
        base_type(_parent),
				serializer_type(*this),
        source_(_parent),
        output_dir_("."),
        prefix_("segments"),
        layer_height_(0.3) {}

      void process()
      {
        using namespace tomo::slice::sampling;
        
        item::Mesh const& _mesh = *source();
        std::vector<Regular<gex::Z>> _samples;
          
        auto&& _transform = _mesh.transformation();
        auto&& _bounds = _mesh.transformedBounds();
        SampleGenerator<TriangleMesh,Regular<Z>>()(_bounds.range(Z),layer_height(),layer_height(),_samples);
        SegmentGenerator<gex::TriangleMesh,Regular<gex::Z>> _segmentGenerator;
        _segmentGenerator(_mesh,_transform,_samples);

        size_t _layerNumber = 0;
        for (auto& _sample : _samples)
        {
          std::stringstream _ss;
          _ss << std::string(output_dir_) << '/' << std::string(prefix_) << "_";
          _ss << std::setw(5) << std::setfill('0') << _layerNumber << ".wkt"; 
          
          std::ofstream _file(_ss.str().c_str());
          for (auto& _segment : _sample.segments())
            _file << boost::geometry::wkt(_segment) << std::endl;
          TOMO_DBG("Writing WKT file to... " << _ss.str())
          ++_layerNumber;
        }
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::tool::ExtractMeshSegments)
