#include <gex/algorithm/offset.hpp>
#include <tomo/tool/Tool.hpp>
#include <tomo/item/Source.hpp>
#include <tomo/item/Sink.hpp>
#include <tomo/item/Regions.hpp>

#include "item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    class Thickening : public Tool<Thickening>
    {
      TOMO_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) sink,
        (Source<tomo::item::Regions>) regions,
        (float) min_detail,
        (float) amount
      )
    public:
      TOMO_TOOL("fdm",Thickening)

      Thickening(parent_type const* _parent) :
        Tool<Thickening>(_parent),
        sink_(_parent),
        min_detail_(0.0),
        amount_(0.5)
      {}

      bool valid() const
      {
        return source() && sink();
      }

      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;
        make(*this,_cfg,"min_detail",{
            {"min","0.0"},
            {"max","1.0"}});
        make(*this,_cfg,"amount",{
            {"min","0.0"},
            {"max","10.0"}});
      }

      void process()
      {
        float _minExtWidth = 0.1;
        float _minDetail = (min_detail() > 0.0) ? float(min_detail()) : _minExtWidth; 
        typedef std::vector<std::pair<size_t,size_t>> indices_type;
        if (sink())
        {
          sink_->clear();
        }

        indices_type _indices;
        tomo::slice::for_each(source()->data(),[&](const fdm::Slice& _slice)
        {
          gex::MultiLineString _details; 
          typedef gex::strategy::offset::MultiWithDetails<gex::Point2> strategy_type;
          gex::offset(_slice.polygon(),strategy_type(_minDetail,1,_details,_indices));

          bool _hasRegions = regions();
          gex::polygon::MultiPolygon _thickened;
          auto _bounds = _slice.polygon().bounds();
          using namespace boost::polygon::operators; 
          gex::for_each<gex::Segment>(_details,[&](const gex::Segment& _segment) 
          {
            if (_hasRegions)
            {
              gex::Bounds3 _segmentBounds = _segment.bounds().expand(gex::Z,_slice.range());
              typedef tomo::item::Regions::node_type node_type;
              std::vector<node_type> _nodes;
              regions()->data().query(gex::index::intersects(_segmentBounds),std::back_inserter(_nodes));
              if (_nodes.empty()) return;
            }
            typedef gex::strategy::offset::NonUniform<gex::Segment> segmentoffset_type;
            auto&& _offsetPolygon = gex::offset(_segment,segmentoffset_type(std::make_pair(amount(),amount())));
            _thickened += gex::polygon::adapt(_offsetPolygon,_bounds);
          });
          _thickened += gex::polygon::adapt(_slice.polygon(),_bounds);

          auto&& _polygons = gex::polygon::adapt(_thickened,_bounds);

          for (auto& _polygon : _polygons)
          {
            _polygon.update();
            sink_->data().insert(fdm::Slice(_slice.range(),_polygon));
          }
        });
        sink_->data().connect();
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::Thickening)
