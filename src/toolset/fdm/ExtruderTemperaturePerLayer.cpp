#include <random>
#include <tomo/item/SourceAndSink.hpp>
#include <tomo/tool/Tool.hpp>
#include "item/Route.hpp"
#include "routing/Event.hpp"

namespace tomo
{
  namespace fdm
  {
    class ExtruderTemperaturePerLayer :
      public Tool<ExtruderTemperaturePerLayer>
    {
      TOMO_PARAMETER_LIST
      (
        (Source<item::Route>) source,
        (Sink<item::Route>) sink,
        (Range) variation,
        (float) z_distance,
        (float) speed_factor,
        (bool) random
      )

    public:
      TOMO_TOOL("fdm",ExtruderTemperaturePerLayer)

      ExtruderTemperaturePerLayer(parent_type const* _parent) : 
        Tool<ExtruderTemperaturePerLayer>(_parent),
        sink_(_parent),
        variation_(-5.0,10.0),
        z_distance_(0.3),
        speed_factor_(1.0),
        random_(true)
      {}

      bool valid() const
      {
        return source(); 
      }
 
      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;
        make(*this,_cfg,"variation",{
            {"min","0.01"},
            {"max","0.50"}});
        make(*this,_cfg,"z_distance",{
            {"min","0.2"},
            {"max","10.0"}});
        make(*this,_cfg,"speed_factor",{
            {"min","0.1"},
            {"max","5.0"}});
        make(*this,_cfg,"random");
      }


      void process()
      {
        sink_->data() = source_->data();
        fdm::Route& _route = sink_->data();
 
        std::random_device rd;
        std::mt19937 gen(rd());
        
        using namespace tomo::routing;

        size_t _eventId = 0, _lastEventId = 0;
        float _lastPos = _route.initial().z();  
        std::uniform_real_distribution<> dis(variation().min(), variation().max());

        bool _var = true;
        iterate(_route,strategy::State(),[&](const State& _lastState, const State& _state)
        {
          if (_state.z() - _lastPos > z_distance()) 
          {
            float _variation = _var ? variation_.min() : variation_.max();
            float _temperature = (random()) ? _state.extruder_temperature() + dis(gen) : _state.extruder_temperature() + _variation;

            if (_var)
            {
              for (size_t i = _lastEventId; i <= _eventId; ++i)
              {
                _route.path()[i].speed() = _route.path()[i].speed() * 0.5;
              }
            }

            _route.eventAtPos<event::Temperature>(_eventId,_temperature);
            _lastPos = _state.z();
            _lastEventId = _eventId;
            _var = !_var;
          }
          ++_eventId;
        });
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::ExtruderTemperaturePerLayer)
