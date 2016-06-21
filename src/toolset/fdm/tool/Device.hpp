#pragma once

#include <tomo/base/Element.hpp>

namespace tomo
{
  namespace fdm
  {
    typedef std::string Token;
    using gex::Scalar;
    using gex::Range;
    using gex::Bounds3;
    using gex::Vec3;

    /// Our FDM Printer Device
    class Device : 
      public base::Element,
      public Serializer<Device>
    {
      TBD_PARAMETER_LIST
      (
        (Range) speed_range,
        (Range) extrusion_width_range,
        (Range) temperature_range,
        (Range) layer_height_range,
        (Bounds3) print_bounds,
        (Float) filament_diameter,
        (Float) extrusion_multiplier,
        (Float) nozzle_diameter,
        (Float) bed_z_offset,
        (Float) retract_distance,
        (Float) retraction,
        (Int) num_extruders
      )
    public:
      typedef Serializer<Device> serializer_type;

      Device(mutable_parent_type _parent) : 
        base::Element(_parent),
        serializer_type(*this),
        speed_range_(5,200),
        extrusion_width_range_(0.1,1.0),
        temperature_range_(150,300),
        layer_height_range_(0.1,0.4),
        print_bounds_(Vec3(0,0,0),Vec3(200,200,200)),
        filament_diameter_(1.75),
        extrusion_multiplier_(1.0),
        nozzle_diameter_(0.35),
        bed_z_offset_(0.1),
        retract_distance_(5.0),
        retraction_(1.0),
        num_extruders_(1) 
      {
        epsilon_ = print_bounds_.size().norm() / 10000.0;
      }

      template<typename T>
      T extrusionWidth(T _extrusionWidth) const
      {
        return (_extrusionWidth <= extrusion_width_range().min()) ? 
          extrusion_width_range().min() : 
          _extrusionWidth;
      }

      template<typename T>
      T speed(T _speed) const
      {
        if (_speed < speed_range().min() ||
            _speed > speed_range().max()) return speed_range().min();
        return _speed;
      } 
      
      template<typename T>
      T temperature(T _temperature) const
      {
        if (_temperature < temperature_range().min() ||
            _temperature > temperature_range().max()) return temperature_range().min();
        return _temperature;
      }

      template<typename T>
      T layer_height(T _layer_height) const
      {
        if (_layer_height < layer_height_range().min() ||
            _layer_height > layer_height_range().max()) return layer_height_range().min();
        return _layer_height;
      }

      TOMO_TYPEID_DECL("device")
    
      TBD_PROPERTY_RO(float,epsilon)
    };

  }
}
