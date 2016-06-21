#include <gex/algorithm/offset.hpp>
#include <tomo/item/SourceAndSink.hpp>
#include <tomo/tool/Tool.hpp>
#include "item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    class Skirt :
      public Tool<Skirt>
    {
      TOMO_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) skirt,
        (Sink<item::SliceGraph>) object,
        (float) height,
        (int) num_layers,
        (float) distance
      )
    public:
      TOMO_TOOL("fdm",Skirt)

      Skirt(parent_type const* _parent) :
        Tool<Skirt>(_parent),
        skirt_(_parent),
        object_(_parent),
        height_(0.0),
        num_layers_(1),
        distance_(20.0)
        {}

      bool valid() const
      {
        return source();
      }

      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;
        make(*this,_cfg,"height",{
            {"min","0.1"},
            {"max","1.0"}});
        make(*this,_cfg,"num_layers",{
            {"min","1"},
            {"max","100"}});
        make(*this,_cfg,"distance",{
            {"min","1"},
            {"max","100"}});
      }

      void process()
      {
        TOMO_DBG("Skirt")
        fdm::SliceGraph const& _sliceGraphIn = source()->data();
        skirt_->clear();
        fdm::SliceGraph& _sliceGraphOut = skirt_->data();

        auto _layerHeight = source()->layer_height();
        if (_layerHeight <= 0.0)
        {
          _layerHeight = _sliceGraphIn.minSliceHeight();
        }

        float _height = height();
        if (_height <= 0.0)
        {
          _height = _layerHeight * (num_layers() + 1);
        }

        using gex::algorithm::offset;

        gex::MultiPolygon _rings;
        tomo::slice::for_each(_sliceGraphIn,[&](const fdm::Slice& _slice)
        {
          _rings.emplace_back(_slice.polygon().boundary());
        });

        gex::Ring _hull;
        gex::algorithm::convex_hull(_rings,_hull);
        _hull.update();
        auto&& _skirt = offset(_hull,gex::strategy::offset::Default(distance()));

        using namespace tomo::slice;
        for_each_layer(_sliceGraphIn,strategy::LayerHeightRangeBottomTop(gex::Range(0,height()),_layerHeight),[&](const fdm::Layer& _layer)
        {
          TOMO_DBG(_skirt.size())
          for (auto& _polygon : _skirt)
          {
            _polygon.update();
            _sliceGraphOut.insert(fdm::Slice(_layer.bounds().range(gex::Z),_polygon));
          }
        });
        _sliceGraphOut.connect();
        
        if (object())
        {
          fdm::SliceGraph& _object = object_->data();
          _object = _sliceGraphIn;
          _object.connect();
        }
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::Skirt)
