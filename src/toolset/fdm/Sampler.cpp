
#include <tomo/slice/sampling.hpp>
#include <tomo/item/Mesh.hpp>
#include <tomo/item/PointCloud.hpp>
#include <tomo/item/SourceAndSink.hpp>

#include "item/SliceGraph.hpp"
#include <tomo/tool/Tool.hpp>

namespace tomo
{
  namespace fdm
  {
    using namespace gex;

    class Sampler : public Tool<Sampler> 
    {
      TOMO_PARAMETER_LIST
      (
        (Source<tomo::item::Mesh>) source,
        (Sink<item::SliceGraph>) sink,
        (float) layer_height,
        (float) first_layer_height,
        (float) simplify
      )

    public:
      TOMO_TOOL("fdm",Sampler)

      Sampler(parent_type const* _parent) :
        Tool<Sampler>(_parent),
        sink_(_parent),
        layer_height_(0.35),
        first_layer_height_(0.35),
        simplify_(0.0) {}

      bool valid() const
      {
        return (source_) && (layer_height_ > 0) && (simplify() >= 0.0);
      }

      void process()
      {
        TOMO_DBG("Sampling with layer height " << layer_height() << " ...")
 
        fdm::SliceGraph& _sliceGraph = sink_->data();
        _sliceGraph.clear();

        sink_->layer_height() = layer_height();

        auto _printBounds = Bounds3(Vec3(0,0,0),Vec3(200,200,200));
        //device()->print_bounds();
        TOMO_DBG("Print bounds: " << _printBounds);

        fdm::slice::Graph::slice_container_type _slices;
         
          gex::TriangleMesh const& _mesh = source()->data();
          auto&& _transform = source()->transformation();
          auto&& _bounds = source()->transformedBounds();
   
          using namespace tomo::slice::sampling;

          /// Generate Samples in range
            std::vector<Regular<Z>> _samples;
            Range _range(0,0);
            gex::algorithm::intersection(
                _bounds.range(Z),
                _printBounds.range(Z),_range);
          
            SampleGenerator<TriangleMesh,Regular<Z>>()(_range,first_layer_height(),layer_height(),_samples);
            sample(_mesh,_transform,simplify(),_samples,_sliceGraph);

        _sliceGraph.connect();
        TOMO_DBG("SliceGraph has " << _sliceGraph.slices().size() << " slices.")
      }
        
      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;
        make(*this,_cfg,"layer_height",{
            {"min","0.01"},
            {"max","0.50"}});
        make(*this,_cfg,"first_layer_height",{
            {"min","0.01"},
            {"max","0.50"}});
        make(*this,_cfg,"simplify",{
            {"min","0.01"},
            {"max","0.05"}});
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::Sampler)
