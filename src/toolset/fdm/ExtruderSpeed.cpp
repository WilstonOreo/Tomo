#include <tomo/routing/optimize_speed.hpp>
#include <tomo/routing/for_each_path.hpp>
#include <tomo/slice/overlapping.hpp>
#include <tomo/item/Sink.hpp>
#include <tomo/item/Source.hpp>
#include <gex/algorithm/within.hpp>
#include <tomo/tool/Tool.hpp>
#include "item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    using gex::Range;
    /// ExtruderSpeed on curvature
    class ExtruderSpeed : public Tool<ExtruderSpeed>
    {
      TOMO_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) sink,
        (float) step,
        (float) bridge_factor,
        (float) curve_factor,
        (float) gamma,
        (float) extrusion_width,
        (Range) speed
      )
    public:
      TOMO_TOOL("fdm",ExtruderSpeed)

      ExtruderSpeed(parent_type const* _parent) :
        Tool<ExtruderSpeed>(_parent),
        sink_(_parent),
        step_(1.0),
        bridge_factor_(0.5),
        curve_factor_(4.0),
        gamma_(1.0),
        extrusion_width_(0.35),
        speed_(5,100) {}

      bool valid() const
      {
        return source() && (bridge_factor() > 0.0) && (bridge_factor() <= 1.0);
      }

      void process()
      {
        using namespace tomo::routing;
        fdm::SliceGraph const& _source = source_->data();
        fdm::SliceGraph& _sink = sink_->data();
        _sink.clear();

        auto _curveLength = extrusion_width() * curve_factor();

        tomo::slice::for_each(_source,[&](const Slice& _slice)
        {
          Slice _newSlice(_slice.range(),_slice.polygon(),_slice.tag());
          for_each_path(_slice,[&](const fdm::Path& _path)
          {
            auto&& _averageSpeed = average_speed(_path);
            Range _speed(
              (speed_.min() + _averageSpeed)*0.5,
              (speed_.max() + _averageSpeed)*0.5);
            auto&& _optPath = optimize_speed(_path,strategy::Curvature(_speed,step(),_curveLength,gamma()));

            auto&& _overlapPolygons = tomo::slice::functor::overlapPolygons<tomo::slice::strategy::Bottom>(_source,_slice);
            for_each_move(_optPath,[&](fdm::Move& _move)
            {
              if (_move.travel()) return;

              if (!gex::within(gex::Point2(_move.x(),_move.y()),_overlapPolygons))
              {
                _move.speed() = _move.speed() * float(bridge_factor());
              }
            });

            _newSlice.insert(_optPath);  
          });
          
          _sink.insert(_newSlice);
        });
        _sink.connect();
      }
    
      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;
        make(*this,_cfg,"step",{
            {"min","1"},
            {"max","10"}});
        make(*this,_cfg,"bridge_factor",{
            {"min","0.1"},
            {"max","0.5"}});
        make(*this,_cfg,"curve_factor",{
            {"min","0.1"},
            {"max","0.5"}});
        make(*this,_cfg,"extrusion_width",{
            {"min","0.01"},
            {"max","1.00"}});
        make(*this,_cfg,"gamma",{
            {"min","2.0"},
            {"max","5.0"}});
        make(*this,_cfg,"speed",{
            {"min","10.0"},
            {"max","120.0"}});
      }

    
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::ExtruderSpeed)
