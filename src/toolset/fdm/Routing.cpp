#include <array> 
#include <tomo/item/SourceAndSink.hpp>
#include <tomo/item/FileSink.hpp>

#include <tomo/routing/centroid.hpp>
#include <tomo/routing/connect.hpp>
#include <tomo/routing/num_moves.hpp>
#include <tomo/tool/Tool.hpp>

#include "routing/Event.hpp"
#include "item/SliceGraph.hpp"
#include "item/Route.hpp"
#include "routing/junction.hpp"

#include <gex/algorithm/join.hpp>

namespace tomo
{
  namespace fdm
  {
    class Routing : public Tool<Routing>
    {
      TOMO_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::Route>) sink,
        (float) temperature,
        (float) travel_speed,
        (bool) fan,
        (float) z_movement_speed,
        (float) z_offset,
        (float) extrusion_multiplier,
        (float) first_layer_speed_factor,
        (float) first_layer_temperature,
        (bool) travel_opt,
        (bool) retraction,
        (float) retract_distance
      )

    public:
      TOMO_TOOL("fdm",Routing)

      Routing(parent_type const* _parent) :
        Tool<Routing>(_parent),
        sink_(_parent),
        temperature_(230.0),
        travel_speed_(60.0),
        fan_(true),
        z_movement_speed_(50.0),
        z_offset_(0.0),
        extrusion_multiplier_(1.0),
        first_layer_speed_factor_(0.5),
        first_layer_temperature_(230.0),
        travel_opt_(true),
        retraction_(true),
        retract_distance_(5.0) {}

      bool valid() const
      {
        if (!source() || !sink()) return false;
        return true;
      }

      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;
        make(*this,_cfg,"temperature",{
            {"min","180"},
            {"max","280"}});
        make(*this,_cfg,"travel_speed",{
            {"min","15"},
            {"max","120"}});
        make(*this,_cfg,"fan");
        make(*this,_cfg,"z_movement_speed",{
            {"min","10"},
            {"max","120"}});
        make(*this,_cfg,"z_offset");
        make(*this,_cfg,"first_layer_speed_factor");
        make(*this,_cfg,"first_layer_temperature");

        make(*this,_cfg,"extrusion_multiplier",{
            {"min","0.1"},
            {"max","10.0"}});
        make(*this,_cfg,"travel_opt");
        make(*this,_cfg,"retraction");
        make(*this,_cfg,"retraction_distance");
      }

      void process()
      {
        fdm::SliceGraph const& _sliceGraph = source_->data();
        if (_sliceGraph.empty() || !tomo::routing::num_moves(_sliceGraph)) return;
        
        fdm::Route& _route = sink_->data();
        _route.clear();
        _route.event<event::Phase>(Phase::INITIALIZATION);
        _route.event<fdm::event::Temperature>(float(first_layer_temperature())); 

        using namespace tomo::routing;

        size_t _layerIdx = 0;
        
        tomo::slice::for_each_layer(_sliceGraph,tomo::slice::strategy::LayerHeightBottomTop(_sliceGraph.minSliceHeight()),[&](const Layer& _layer)
        {
          size_t _routeSizeBefore = _route.size(); 
          connectLayer(_layer,_route);
          /// Handle speed and temperature for first layer
          if (_layerIdx == 0)
          {
            auto& _path = _route.path();
            for (auto it = _path.begin() + _routeSizeBefore; it != _path.begin() + _route.size(); ++it)
            {
              it->speed() = it->speed() * float(first_layer_speed_factor());
            }
            if (fan())
            {
              _route.event<fdm::event::Fan>(255);
            }
            _route.event<fdm::event::Temperature>(float(temperature()));
          }
          ++_layerIdx;
        });

        // Offset end extrusion width
        gex::for_each<Move>(_route.path(),[&](Move& _move)
        {
           _move.extrusionWidth(_move.extrusionWidth() * extrusion_multiplier()); 
           _move.z() += float(z_offset());
        });
        handleRetraction(_route);

        TOMO_DBG(_route.path().size()) 
       
        _route.path().back().setTravel();

        _route.event<event::Retract>(); 
        _route.event<event::Phase>(Phase::FINALIZATION);
        _route.event<event::Home>(fdm::Origin::X);
        _route.event<event::Fan>(0);
        _route.event<event::Temperature>(0);
        _route.event<fdm::event::Motors>(false);

/*
        // Move extruder upwards on last point
        gex::Point3 _lastPoint = _route.path().back();
        _lastPoint.z() += 0.5;
        _route.path().emplace_back(_lastPoint,float(z_movement_speed()),0.0);

*/
      }
    private:
      void handleRetraction(fdm::Route& _route) 
      {
        float _travelDist = 0.0; 
        typedef std::pair<Move,Move> move_pair;
        size_t _eventId = 0;
        size_t _lastTravel = 0;
        gex::for_each<move_pair>(_route.path(),[&](const Move& _first, const Move& _second)
        {
          if (_first.travel())
          {
            _travelDist += gex::distance(_first,_second);
          } else
          {
            if (_travelDist >= retract_distance())
            {
              _route.eventAtPos<fdm::event::Retract>(_lastTravel+2);
            }
            if (_second.travel())
            {
              _lastTravel = _eventId;
            }
            _travelDist = 0;
          } 
          ++_eventId;
        });
      }


      typedef std::map<fdm::Path const*,fdm::Slice const*> pathinfo_type;

      template<typename STRATEGY>
      pathinfo_type connectLayer(const fdm::Layer& _layer, STRATEGY _s, std::vector<fdm::Path>& _paths)
      {
        pathinfo_type _pathInfo;
        _paths.resize(_layer.slices().size());
        for (auto& _slice : _layer.slices())
        {
          _paths.push_back(tomo::routing::connect(*_slice,_s));
          _pathInfo.insert({&_paths.back(),_slice});
        }
        return _pathInfo;
      }
      
      void connectLayer(const fdm::Layer& _layer, fdm::Route& _route)
      {
        using namespace tomo::routing;
        float _travelSpeed = travel_speed(); 
        typedef fdm::routing::junction::Straight junction_type;
        typedef fdm::routing::junction::ZAxis zjunction_type;
        typedef strategy::connect::NoOpt<junction_type> noopt_strategy_type;
        typedef strategy::connect::NearestInsertion<junction_type> opt_strategy_type;        
        typedef std::pair<fdm::Path,Scalar> path_height_pair;

        junction_type _junction(_travelSpeed);

        std::vector<fdm::Path> _paths;
        pathinfo_type _pathInfo;

        /// 1st Step: Connect each pathgroup to a single path and get height of each path group        
        if (travel_opt())
        {
          _pathInfo = connectLayer(_layer,opt_strategy_type(_junction),_paths);
        } else
        {
          _pathInfo = connectLayer(_layer,noopt_strategy_type(_junction),_paths);
        }

        gex::Point3 _startPoint(0.0,0.0,0.0);
        if (!_route.empty())
        {
          _startPoint = gex::Point3(_route.path().back());
        }
        
        fdm::Path _path;
        
        /// 2nd Step: Connect paths together to a single one and append it to route
        if (travel_opt())
        {
          std::vector<fdm::Path const*> _connections;
          gex::algorithm::functor::detail::connection_list(_startPoint,_paths,_connections);
          for (auto& _connection : _connections)
          {
            if (_pathInfo.count(_connection) == 0) continue;
            auto& _p = *_connection;
            if (_p.empty()) return;

            auto& _pathGroup = _pathInfo[&_p];
            _route.eventAtPos<fdm::event::LayerHeight>(_route.size() + _path.size(),_pathGroup->height());
            gex::connect(_path,_p,_junction);
          }
        } else
        {
          // No travel optimization
          for (auto& _p : _paths)
          {
            if (_pathInfo.count(&_p) == 0) continue;
            auto& _pathGroup = _pathInfo[&_p];
            _route.eventAtPos<fdm::event::LayerHeight>(_route.size() + _path.size(),_pathGroup->height());
            gex::connect(_path,_p,_junction);
          }
        }

        if (_path.empty()) return;
        
        /// If route is empty, add start point 
        if (_route.empty())
        {
          _route.path().emplace_back(gex::Point3(0.0,0.0,_path.front().z()),_travelSpeed,0.0);
        }

        _route.event<fdm::event::Phase>(Phase::LAYER);
        _route.event<fdm::event::NextLayer>();

        gex::connect(_route.path(),_path,zjunction_type(_travelSpeed,z_movement_speed()));
        _route.path().front().speed(_travelSpeed);
        _route.path().at(1).speed(_travelSpeed);
      }

    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::Routing)
