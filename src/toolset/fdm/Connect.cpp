#include <tomo/item/SourceAndSink.hpp>
#include <tomo/tool/Tool.hpp>
#include <tomo/routing/for_each_move.hpp>
#include <tomo/routing/connect.hpp>
#include "item/SliceGraph.hpp"
#include "routing/junction.hpp"

namespace tomo
{
  namespace fdm
  {
    /// Connects all path in a slicegraph into a single one
    class Connect : public Tool<Connect>
    {
      TOMO_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) sink,
        (float) travel_speed,
        (bool) travel_opt,
        (int) tag
      )
    public:
      TOMO_TOOL("fdm",Connect)

      Connect(parent_type const* _parent) :
        Tool<Connect>(_parent),
        sink_(_parent),
        travel_speed_(60.0),
        travel_opt_(false),
        tag_(0) {}

      bool valid() const
      {
        return source() && sink();
      }
      
      void process()
      {
        fdm::SliceGraph const& _source = source_->data();
        fdm::SliceGraph& _sink = sink_->data();
        _sink.clear();
        typedef std::vector<Path> paths_type;

        using namespace tomo::routing;

        typedef fdm::routing::junction::Straight junction_type;
        typedef strategy::connect::NoOpt<junction_type> noopt_strategy_type;
        typedef strategy::connect::NearestInsertion<junction_type> opt_strategy_type;

        float _travelSpeed = travel_speed();

        gex::Point3 _startPoint(0.0,0.0,0.0);

        tomo::slice::for_each_layer(_source,tomo::slice::strategy::LayerHeightBottomTop(_source.minSliceHeight()),[&](const Layer& _layer)
        {
          fdm::Path _path;
          auto _j = junction_type(_travelSpeed);
          if (travel_opt())
          {
            connectLayer(_layer,_startPoint,noopt_strategy_type(_j),_path);
          } else
          {
            connectLayer(_layer,_startPoint,opt_strategy_type(_j),_path);
          }
          if (_path.empty()) return;
          _startPoint = gex::Point3(_path.back());

          Slice _slice(_layer.bounds().range(gex::Z),pathPolygon(_path),tag());
          _slice.insert(_path);
          _sink.insert(_slice);
        });
      }

      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;
        make(*this,_cfg,"travel_speed",{
            {"min","10.0"},
            {"max","120.0"}});
        make(*this,_cfg,"travel_opt");
      }


    private:
      template<typename STRATEGY>
      void connectLayer(const fdm::Layer& _layer, const gex::Point3& _point, STRATEGY _s, fdm::Path& _path)
      {
        std::vector<fdm::Path> _paths;
        _paths.resize(_layer.slices().size());
        tomo::slice::for_each(_layer,[&](const Slice& _slice)
        {
          _paths.push_back(tomo::routing::connect(_slice,_s));
        });

        std::vector<fdm::Path const*> _connections;
        gex::algorithm::functor::detail::connection_list(_point,_paths,_connections);
        
        for (auto& _connection : _connections)
        {
          auto& _p = *_connection;
          if (_p.empty()) return;
        }
      }

      template<typename PATH> 
      gex::Polygon pathPolygon(const PATH& _path)
      {
        gex::LineString _lineString;
        using namespace tomo::routing;
        for_each_move(_path,[&](const fdm::Move& _move)
        {
          _lineString.emplace_back(_move.x(),_move.y());
        });
        gex::Polygon _hull;
        gex::algorithm::convex_hull(_lineString,_hull);
        return _hull;
      }      
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::Connect)
