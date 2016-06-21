#include <gex/algorithm/offset.hpp>
#include <gex/algorithm/difference.hpp>
#include <tomo/slice/for_each_layer.hpp>
#include <tomo/item/SourceAndSink.hpp>
#include <tomo/tool/Tool.hpp>
#include "item/SliceGraph.hpp"


namespace tomo
{
  namespace fdm
  {
    class Brim : public Tool<Brim>
    {
      TOMO_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) brim,
        (Sink<item::SliceGraph>) object,
        (float) height,
        (float) width
      )
    public:
      TOMO_TOOL("fdm",Brim)

      Brim(parent_type const* _parent) :
        Tool<Brim>(_parent),
        brim_(_parent),
        object_(_parent),
        height_(0.5),
        width_(4.0)
      {}

      bool valid() const
      {
        return source();
      }

      void process() 
      {
        TOMO_DBG("Brim")
        fdm::SliceGraph const& _sliceGraphIn = source_->data();

        using gex::algorithm::offset;
        using gex::Range;

        auto _layerHeight = source()->layer_height();
        if (_layerHeight <= 0.0)
        {
          _layerHeight = _sliceGraphIn.minSliceHeight();
        }

        using namespace tomo::slice;

        if (brim())
        {
          fdm::SliceGraph& _brim = brim_->data();
          _brim.clear();
          /// 1st step: Generate bottom brim polygon with height
          for_each_layer(_sliceGraphIn,strategy::LayerHeightRangeBottomTop(Range(0,height()),_layerHeight),
              [&](const fdm::Layer& _layer)
          {
            gex::MultiPolygon _offsetPolygons, _brimPolygons; 
            tomo::slice::for_each(_layer,[&](const fdm::Slice& _slice)
            { 
              _offsetPolygons.push_back(_slice.polygon().boundary());
            });
          
            _brimPolygons = offset(_offsetPolygons,gex::strategy::offset::Default(width()));
            for (auto& _polygon : _brimPolygons)
            {
              _polygon.update();
              _brim.insert(fdm::Slice(_layer.bounds().range(gex::Z),_polygon));
            }
          });
          _brim.connect();
        }
      
        if (object())
        {
          fdm::SliceGraph& _object = object_->data();
          _object = _sliceGraphIn;
          _object.connect();
        }
      
      }

      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;
        make(*this,_cfg,"height",{
            {"min","0.00"},
            {"max","4.00"}});
        make(*this,_cfg,"width",{
            {"min","0.50"},
            {"max","10.00"}});
      }
    };        
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::Brim)
