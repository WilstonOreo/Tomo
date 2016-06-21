#pragma once
#include <tomo/routing/State.hpp>

#include "Move.hpp"

namespace tomo
{
  namespace fdm
  {
    using gex::Scalar;

    enum class Movement { NONE, RAPID, CONTROLLED, ARC };
    enum class Positioning { ABSOLUTE, RELATIVE };
    enum class LengthUnit { MILLIMETER, INCH };
    enum class Origin { NONE, ALL, X, Y, Z };
    enum class Phase { INITIALIZATION, LAYER, NEXT_LAYER, FINALIZATION, NONE };
    /// The State defines all properties an FDM printer can hold at a point in time
    struct State : tomo::routing::State<Move>
    {
      typedef Move move_type;
      typedef move_type::pos_type pos_type;
      typedef tomo::routing::State<Move> inherited_type;
      typedef typename inherited_type::time_type time_type;
      
      State(const Move& _move = Move(gex::Point3(0.0,0.0,0.0)), const time_type& _time = 0.0) :
        inherited_type(_move,_time),
        extruder_temperature_(220.0),
        extruder_length_(0.0),
        filament_length_(0.0),
        layerId_(0),
        layerHeight_(0),
        fan_(0),
        phase_(Phase::NONE),
        origin_(Origin::ALL),
        positioning_(Positioning::ABSOLUTE),
        extrusion_positioning_(Positioning::ABSOLUTE),
        unit_(LengthUnit::MILLIMETER),
        motors_(true),
        retract_(false) {}

      TBD_PROPERTY_REF(Scalar,extruder_temperature)
      TBD_PROPERTY_REF(Scalar,extruder_length) 
      TBD_PROPERTY_REF(Scalar,filament_length) 
      TBD_PROPERTY_REF(int,layerId) 
      TBD_PROPERTY_REF(Scalar,layerHeight)
      TBD_PROPERTY_REF(int,fan) 
      TBD_PROPERTY_REF(Phase,phase) 
      TBD_PROPERTY_REF(Origin,origin)  
      TBD_PROPERTY_REF(Positioning,positioning)
      TBD_PROPERTY_REF(Positioning,extrusion_positioning)
      TBD_PROPERTY_REF(LengthUnit,unit)
      TBD_PROPERTY_REF(bool,motors)
      TBD_PROPERTY_REF(bool,retract)

      friend std::ostream& operator<<(std::ostream& _os, const State& _state)
      {
        _os << _state.x() << ",";
        _os << _state.y() << ",";
        _os << _state.z() << ",";
        _os << _state.speed() << ",";
        _os << _state.extrusionWidth() << ",";
        _os << _state.retract() << ",";
        _os << _state.time() << ",";
        _os << _state.extruder_temperature() << ",";
        _os << _state.extruder_length() << ",";
        _os << _state.filament_length() << ",";
        _os << _state.layerId() << ",";
        _os << _state.layerHeight() << ",";
        _os << int(_state.phase()) << ",";
        _os << int(_state.origin()) << ",";
        _os << int(_state.positioning()) << ",";
        _os << int(_state.extrusion_positioning()) << ",";
        _os << int(_state.unit()) << ",";
        _os << _state.fan() << ",";
        _os << _state.motors() << ",";
        return _os;
      }
    };
  }
}


/// Distance between two states 
namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<>
      struct SqrDistance<tomo::fdm::State,tomo::fdm::State,Scalar>
      {
        typedef tomo::fdm::State state_type;
        typedef state_type::move_type move_type;

        inline Scalar operator()(const state_type& _prev, const state_type& _next)
        {
          return sqrDistance(move_type(_prev),move_type(_next));
        }
      };
    }
  }
}

