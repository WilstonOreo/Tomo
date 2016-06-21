#include <tbd/seq_op.h>
#include <tomo/item/Sink.hpp>
#include <tomo/item/Source.hpp>
#include <tomo/tool/Tool.hpp>

#include "tool/Pattern.hpp"
#include "routing/MoveMaker.hpp"
#include "item/SliceGraph.hpp"

#include <tomo/slice/for_each_layer.hpp>

namespace tomo
{
  namespace fdm
  {
    class PatternInfill : public Tool<PatternInfill>
    {
      enum Pattern { LINEAR, HEXAGON, CONCENTRIC };

      TOMO_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) sink,
        (int) pattern,
        (float) speed,
        (float) extrusion_width,
        (float) angle,
        (float) density,
        (bool) connect,
        (float) distance,
        (bool) alternate_angle,
        (float) layer_height,
        (int) path_tag
      )
    public:
      TOMO_TOOL("fdm",PatternInfill)

      PatternInfill(parent_type const* _parent) :
        Tool<PatternInfill>(_parent),
        sink_(_parent),
        pattern_(LINEAR),
        speed_(50.0),
        extrusion_width_(0.5),
        angle_(45.0),
        density_(1.0),
        connect_(false),
        distance_(1.2),
        alternate_angle_(true),
        layer_height_(0.3),
        path_tag_(0)
      {}

      bool valid() const
      {
        return source();
      }

      void process()
      {
        TOMO_DBG("Infill")
        fdm::SliceGraph const& _sliceGraphIn = source()->data();
        sink_->clear();

        std::unique_ptr<pattern::Interface> _even;
        std::unique_ptr<pattern::Interface> _odd;

        float _offsetAngle = alternate_angle() ? 90 : 0;

        switch (pattern())
        {
          case LINEAR:
            _even.reset(new pattern::Linear(angle(),density(),connect()));
            _odd.reset(new pattern::Linear(angle() + _offsetAngle,density(),connect()));
            break;
          case HEXAGON:
            _even.reset(new pattern::Hexagon(angle(),density(),distance()));
            _odd.reset(new pattern::Hexagon(angle() + _offsetAngle,density(),distance()));
            break;
          case CONCENTRIC:
            _even.reset(new pattern::Concentric(density()));
            _odd.reset(new pattern::Concentric(density()));
            break;
        };

        // Unknown Pattern type
        if (!_even || !_odd) 
        {
          TOMO_DBG("Unknown pattern type!");
          return;
        }

        int i = 0;
        float _layerHeight = layer_height() <= 0.0 ? source()->layer_height() : 
          layer_height();

        size_t _layerId = 0;
        tomo::slice::for_each_layer(_sliceGraphIn,tomo::slice::strategy::LayerHeightBottomTop(_layerHeight),
            [&](const fdm::Layer& _layer)
        {
          ++_layerId;
          pattern::Interface* _pattern = _layerId % 2 ? _even.get() : _odd.get();
          auto&& _bounds = _sliceGraphIn.bounds().project(Slice::axis());
          tomo::slice::for_each(_layer,[&](const fdm::Slice& _slice)
          {
            auto& _polygon = _slice.polygon();
            auto&& _lineStrings = _pattern->generate(_bounds,_polygon);
            Slice _pathSlice(_slice.range(),_polygon,path_tag());
            routing::generate(_lineStrings,fdm::MoveMaker(_slice,speed(),extrusion_width()),_pathSlice);
            sink_->data().insert(_pathSlice);
          });
        });
      }
        
      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;

        make(_cfg,"pattern",{
                { "type", "list"},
                { "items", "linear,hexagon,concentric"}});
        
        switch (pattern())
        {
          case LINEAR:
            make(*this,_cfg,"angle");
            make(*this,_cfg,"density");
            make(*this,_cfg,"connect");
            break;
          case HEXAGON:
            make(*this,_cfg,"angle");
            make(*this,_cfg,"density");
            make(*this,_cfg,"distance");
            break;
          case CONCENTRIC:
            make(*this,_cfg,"density");
            break;
        };
        
        make(*this,_cfg,"speed");
        make(*this,_cfg,"extrusion_width");
        make(*this,_cfg,"layer_height");
        make(*this,_cfg,"alternate_angle");
        make(*this,_cfg,"path_tag");
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::PatternInfill)
