#pragma once

namespace tomo
{
  namespace fdm
  {
    template<typename DEVICE>
    struct Extruder
    {
      Extruder(DEVICE const& _device) :
        position_(0.0),
        filamentLength_(0.0),
        device_(_device) {}

      inline void reset()
      {
        position_ = 0.0;
      }

      inline bool origin() const
      {
        return position_ == 0.0;
      }

      inline void snort(float _amount = 1.0)
      {
        if (_amount <= 0.0) return; 
        position_ -= _amount;
      }

      inline void squirt(float _amount = 1.0)
      {
        if (_amount <= 0.0) return; 
        position_ += _amount;
        filamentLength_ += _amount; 
      }

      template<typename A, typename B> 
      void extrude(const A& _a, const B& _b, float _height)
      {
        float _dist = gex::distance(gex::Point3(_a),gex::Point3(_b));
        // Special case if two points are same:
        // Extrude a small cylinder with r = extrusionWidth/2.0  
        auto _r1 = _a.extrusionWidth();
        auto& _r2 = device_.filament_diameter();
        if (_b.travel() && (_dist <= 0.01))
        {
          squirt(device_.extrusion_multiplier()*_r1 * _r1 * _height);
          return;
        }
        /// Extrusion Length
        float _eL = device_.extrusion_multiplier() * _dist * _height * _r1 / (_r2 * _r2);
        squirt(_eL);
      }

      TBD_PROPERTY_RO(float,position)
      TBD_PROPERTY_RO(float,filamentLength)
    private:
      DEVICE const& device_;
    };

    struct NullExtruder;

    template<typename EXTRUDER = NullExtruder>
    struct StateIncrementer
    {
      StateIncrementer(EXTRUDER& _extruder) :
        extruder_(_extruder) {}

      template<typename STATE>
      void operator()(const STATE& _prev, STATE& _next)
      {
        auto _dist = gex::distance(_prev,_next);
        _next.extruder_length() = extruder_.position();
        _next.filament_length() = extruder_.filamentLength();
        if (_prev.retract())
        {
          _next.retract(false);
        }
        _next.time() = _prev.time() + _dist / _next.speed();
        if (_dist > 0.0)
          _next.origin(Origin::NONE);
      }

      EXTRUDER& extruder_;
    };

    template<>
    struct StateIncrementer<NullExtruder>
    {
      template<typename STATE>
      void operator()(const STATE& _prev, STATE& _next)
      {
        auto _dist = gex::distance(_prev,_next);
        if (_prev.retract())
        {
          _next.retract(false);
        }
        _next.time() = _prev.time() + _dist / _next.speed();
        if (_dist > 0.0)
          _next.origin(Origin::NONE);
      }
    };


    
  }
}


