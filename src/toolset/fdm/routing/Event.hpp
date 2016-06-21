#pragma once

#include <tomo/routing/Event.hpp>
#include "State.hpp"

namespace tomo
{
  namespace fdm
  {
    typedef routing::Event<State> Event;
    typedef routing::EventList<Event> EventList;

    namespace event
    {
      enum class Id 
      {
        Temperature, Fan, Motors, Home, NextLayer, Retract, ResetExtruder,LayerHeight, Phase, Squirt
      };

#define TOMO_FDM_EVENT_ID(name)\
      size_t id() const { return size_t(Id::name); }

      struct Temperature : fdm::Event 
      {
        Temperature(Scalar _temperature) : 
          temperature_(_temperature) {}
        void invoke(State& _state) const
        {
            _state.extruder_temperature(temperature_);
        }
        TOMO_FDM_EVENT_ID(Temperature)
        TBD_PROPERTY_REF(Scalar,temperature)
      };
      
      struct Fan : fdm::Event 
      {
        Fan(int _fan) : fan_(_fan) {}
 
        void invoke(State& _state) const
        {
            _state.fan(fan_);
        }
        TOMO_FDM_EVENT_ID(Fan)
        TBD_PROPERTY_REF(int,fan)
      };

      struct Motors : fdm::Event
      {
        Motors(bool _motors) : motors_(_motors) {}

        void invoke(State& _state) const
        {
          _state.motors(motors_);
        }
        TOMO_FDM_EVENT_ID(Motors)
        TBD_PROPERTY_REF(bool,motors)
      };

      struct Home : fdm::Event
      {
        Home(const Origin& _origin) : 
          origin_(_origin) {}

        void invoke(State& _state) const
        {
          _state.origin(origin_);
        }
        TOMO_FDM_EVENT_ID(Home)
        TBD_PROPERTY_REF(Origin,origin)
      };

      struct NextLayer : fdm::Event
      {
        void invoke(State& _state) const
        {
          _state.retract(true);
          _state.extruder_length() = 0.0;
          ++_state.layerId();
        }
        TOMO_FDM_EVENT_ID(NextLayer)
      };

      struct Retract : fdm::Event
      {
        Retract() {}

        void invoke(State& _state) const
        {
          _state.retract(true);
        }
        TOMO_FDM_EVENT_ID(Retract)
      };

      struct Squirt : fdm::Event
      {
        Squirt(const Scalar& _amount) : 
          amount_(_amount) {}
        
        void invoke(State& _state) const
        {
          _state.extruder_length() += amount_;
        }
        TOMO_FDM_EVENT_ID(Squirt)
        TBD_PROPERTY_REF(Scalar,amount)
      };
      
      struct ResetExtruder : fdm::Event
      { 
        void invoke(State& _state) const
        {
          _state.extruder_length(0.0);
        }
        TOMO_FDM_EVENT_ID(ResetExtruder)
      };

      struct LayerHeight : fdm::Event
      {
        LayerHeight(const Scalar& _layerHeight) :
          layerHeight_(_layerHeight) {}

        void invoke(State& _state) const
        {
          _state.layerHeight(layerHeight_);
        }

        TOMO_FDM_EVENT_ID(LayerHeight)
        TBD_PROPERTY_REF(Scalar,layerHeight)
      };

      struct Phase : fdm::Event
      { 
        Phase(const fdm::Phase& _phase) :
          phase_(_phase) {}

        void invoke(State& _state) const
        {
          _state.phase(phase_);
        }
        TOMO_FDM_EVENT_ID(Phase)
        TBD_PROPERTY_REF(fdm::Phase,phase)
      };
    }
  }
}

