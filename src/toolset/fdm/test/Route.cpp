#include <tomo/tool/Chain.hpp>
#include "../item/Route.hpp"
#include "../routing/Event.hpp"

#include <tomo/routing/generate.hpp>
#include <tomo/routing/iterate.hpp>

using namespace tomo;

int main(int argc, char* argv[])
{
  using namespace fdm;

  Route _route;

  const int _numLayers = 40;
  float _layerHeight = 0.35;

  typedef Move::pos_type Pos;
  Pos _center(100,100,0);

  float _travelSpeed = 30.0;
  float _speed = 15.0;
  float _firstLayerSpeedFactor = 0.5;

  GCodes _gcodes;
  Device _device(nullptr);

  float _extrusionWidth = 0.35 * 1.4;
  float _zPos = 0;

  _route.event<event::Phase>(Phase::INITIALIZATION);
  _route.event<event::Temperature>(240);
  _route.event<event::Retract>();
  _route.path().emplace_back(Pos(0,0,_layerHeight),_travelSpeed,0.0);

  auto _square = [&](float _sideLength, float _speed)
  {
    _route.event<event::Retract>();
    _route.path().emplace_back(_center + Pos(-_sideLength,-_sideLength,_zPos),_travelSpeed,0.0);
    _route.path().emplace_back(_center + Pos(_sideLength,-_sideLength,_zPos),_speed,_extrusionWidth);
    _route.path().emplace_back(_center + Pos(_sideLength,_sideLength,_zPos),_speed,_extrusionWidth);
    _route.path().emplace_back(_center + Pos(-_sideLength,_sideLength,_zPos),_speed,_extrusionWidth);
    _route.path().emplace_back(_center + Pos(-_sideLength,-_sideLength,_zPos),_speed,_extrusionWidth);
    _route.path().emplace_back(_route.path().back(),_travelSpeed,0.0);
  };

  for (int _layer = 0; _layer < _numLayers; ++_layer)
  {
    float _s = (_layer == 0) ? _speed * _firstLayerSpeedFactor : _speed;
    _zPos += _layerHeight;
    _route.event<event::NextLayer>();
    _route.path().emplace_back(Pos(_route.path().back().x(),_route.path().back().y(),_zPos),_travelSpeed,0.0); 
    _route.event<event::Phase>(Phase::LAYER);
    if (_layer == 1)
    {
      _route.event<event::Temperature>(230);
      _route.event<event::Fan>(255);
    }
    _square(20,_s);
    _square(10,_s);
    _square(5,_s); 
  }

  _route.event<event::Phase>(Phase::FINALIZATION);
  _route.event<event::Fan>(0);
  _route.event<event::Temperature>(0);
  _route.event<event::Home>(Origin::X);
  _route.event<event::Motors>(false);

  return EXIT_SUCCESS;
}
