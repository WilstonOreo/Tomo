#include <gex/algorithm/offset.hpp>
#include <gex/algorithm/difference.hpp>
#include <tomo/slice/for_each_layer.hpp>
#include <tomo/item/Source.hpp>
#include <tomo/item/Sink.hpp>
#include <tomo/routing/for_each_path.hpp>
#include <tomo/tool/Tool.hpp>
#include "item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    class Raft : 
      public Tool<Raft>
    {
      TBD_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) raft,
        (Sink<item::SliceGraph>) object,
        (float) layer_height,
        (float) height,
        (float) offset,
        (float) width
      )
    public:
      TOMO_TOOL("fdm",Raft)

      Raft(parent_type const* _parent) :
        Tool<Raft>(_parent),
        raft_(_parent),
        object_(_parent),
        layer_height_(0.35),
        height_(1.0),
        offset_(0.0),
        width_(4.0)
      {}

      bool valid() const
      {
        return source() && raft();
      }


      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;
        make(*this,_cfg,"layer_height",{
            {"min","0.1"},
            {"max","10.0"}});
        make(*this,_cfg,"offset",{
            {"min","0.01"},
            {"max","1.00"}});
        make(*this,_cfg,"width",{
            {"min","1.0"},
            {"max","10.0"}});
        make(*this,_cfg,"height",{
            {"min","0.10"},
            {"max","3.5"}});
      }

      void process() 
      {
        TOMO_DBG("Raft")
        fdm::SliceGraph const& _sliceGraphIn = source_->data();
        fdm::SliceGraph& _sliceGraphOut = raft_->data();
        raft_->clear();

        using gex::algorithm::offset;
        using gex::Range;
        using namespace tomo::slice;

        gex::polygon::MultiPolygon _rings;

        gex::Bounds2 _bounds = _sliceGraphIn.bounds().project(gex::Z);
        /// Generate raft polygon with height
        for_each_layer(_sliceGraphIn,strategy::LayerHeightRangeBottomTop(Range(0,height()),layer_height()),
            [&](const fdm::Layer& _layer)
        {
          for_each(_layer,[&](const fdm::Slice& _slice)
          {
            using namespace boost::polygon::operators;
            _rings += gex::polygon::adapt(gex::convert<gex::MultiPolygon>(_slice.polygon().boundary()),_bounds);
          });
        });

        auto&& _polygons = gex::polygon::adapt(_rings,_bounds);
        _polygons.update();
        auto&& _raft = offset(_polygons,gex::strategy::offset::Default(width()));
        size_t _numLayers = 0;
        for_each_layer(_sliceGraphIn,strategy::LayerHeightRangeBottomTop(Range(0,height()),layer_height()),
            [&](const fdm::Layer& _layer)
        {
          for (auto& _polygon : _raft)
          {
            _polygon.update();
            _sliceGraphOut.insert(fdm::Slice(_layer.bounds().range(gex::Z),_polygon));
          }
          _numLayers++;
        }); 
        _sliceGraphOut.connect();
      
        /// If there as an object, move it upwards
        if (object())
        {
          fdm::SliceGraph& _obj = object_->data();
          _obj.clear();

          gex::Scalar _offset = _numLayers * layer_height() - offset_;
          for_each(_sliceGraphIn,[&](const Slice& _slice)
          {
            auto _newSlice = _slice;
            _newSlice.range() += _offset; 
            _obj.insert(_newSlice);
          });
          _obj.connect();
        }
      
      }
    };        
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::Raft)
