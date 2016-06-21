#include <tomo/slice/overlapping.hpp>
#include <gex/algorithm/intersection.hpp>
#include <tomo/item/SourceAndSink.hpp>
#include <tomo/tool/Tool.hpp>
#include "item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    class Support : public Tool<Support>
    {
      TOMO_PARAMETER_LIST
      ( 
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) sink,
        (float) angle
      )
    public:
      TOMO_TOOL("fdm",Support)

      Support(parent_type const* _parent) :
        Tool<Support>(_parent),
        sink_(_parent),
        angle_(45.0) {}

      bool valid() const
      {
        return source() && sink(); 
      }

      void process()
      {
        TOMO_DBG("Generating support...")
      
        fdm::SliceGraph const& _sliceGraphIn = source_->data();
        fdm::SliceGraph& _sliceGraphOut = sink_->data();
        sink_->clear();

        using namespace tomo::slice;
        auto _bounds = _sliceGraphIn.bounds().project(gex::Z);

        gex::Scalar _layerHeight = _sliceGraphIn.maxSliceHeight();
        if (_layerHeight <= 0.0) return;
        gex::polygon::MultiPolygon _layerOverlap;
        gex::Scalar _offset = -_layerHeight * tan(gex::deg2rad(angle()));

        for_each_layer(_sliceGraphIn,strategy::LayerHeightRangeTopBottom(gex::Range(0.0,_sliceGraphIn.bounds().max()(gex::Z)),_layerHeight),[&](const fdm::Layer& _layer)
        {
          using namespace boost::polygon::operators;
          gex::polygon::MultiPolygon _layerPolygons;
          for_each(_layer,[&](const fdm::Slice& _slice)
          {
            TOMO_DBG("Slice range: " << _slice.range())
            gex::polygon::MultiPolygon&& _slicePolygon = gex::polygon::adapt(gex::convert<gex::MultiPolygon>(_slice.polygon()),_bounds);
            _layerPolygons += _slicePolygon;
          });
          
          _layerOverlap -= _layerPolygons;
          
          for_each(_layer,[&](const fdm::Slice& _slice)
          {
            functor::polygonSliceDiff(_sliceGraphIn.indexesToPointers(_slice.lower()),_bounds,_layerPolygons);
          });

          _layerOverlap += _layerPolygons;
          auto&& _layerRange = _layer.bounds().range(gex::Z);
          _layerRange -= _layerHeight;
          TOMO_DBG(_layerRange << "\t" << _layerOverlap.size() << "\t" << _layerPolygons.size())
        
          gex::MultiPolygon&& _support = gex::polygon::adapt(_layerOverlap,_bounds);   

          _support.update();
          //auto&& _offsetPolygons = gex::offset(_support,_offset);
          
          for (auto& _polygon : _support)
          {
            _polygon.update();
            _sliceGraphOut.insert(fdm::Slice(_layerRange,_polygon));
          }
        });

        _sliceGraphOut.connect();
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::Support)
