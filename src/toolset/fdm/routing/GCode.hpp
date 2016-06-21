#pragma once

#include <tomo/routing.hpp>
#include "State.hpp"
#include <tomo/base/Serialization.hpp>
#include <tomo/routing/gcode/Field.hpp>

namespace tomo
{
  namespace fdm
  {
    using base::Token;

    class GCodes :
      public tbd::Serializer<GCodes>
    {
      typedef routing::gcode::Field GCodeField;

      TBD_PARAMETER_LIST 
      (
        (GCodeField) controlled_movement,
        (GCodeField) home,
        (GCodeField) abs_positioning,
        (GCodeField) rel_positioning,
        (GCodeField) extrusion_abs_positioning,
        (GCodeField) set_position,
        (GCodeField) dwell,
        (GCodeField) unit_millimeter,
        (GCodeField) unit_inch,
        (GCodeField) set_temperature,
        (GCodeField) disable_motors,
        (GCodeField) fan,
        (GCodeField) fan_off
      )
    public:
      GCodes() : 
        controlled_movement_("G1"),
        home_("G28"),
        abs_positioning_("G90"),
        rel_positioning_("G91"),
        extrusion_abs_positioning_("M82"),
        set_position_("G92"),
        dwell_("G4"),
        unit_millimeter_("G21"),
        unit_inch_("G20"),
        set_temperature_("M104"),
        disable_motors_("M84"),
        fan_("M106"),
        fan_off_("M107") {}
    };

    namespace gcode
    {
      typedef tomo::routing::gcode::Command Command;
      typedef tomo::routing::gcode::Container<State,Command> Container;

      struct Generator
      {
        Generator(const GCodes& _gcodes) :
          gcodes_(_gcodes) {}

        void clear()
        {
          commands_.clear();
        }

        void initialization(const State& _prev, const State& _next)
        {
          if (_next.phase() != Phase::INITIALIZATION) return;
          unit(_prev,_next);
          temperature(_prev,_next);
          home(_prev,_next);
          fan(_prev,_next);
          positioning(_prev,_next);
          set_position(0.0);
          extrusion_positioning(_prev,_next);
          set_position(0.0);
        }

        void finalization(const State& _prev, const State& _next)
        {
          if (_next.phase() != Phase::FINALIZATION) return;
          home(_prev,_next);
          motors(_prev,_next);
        }

        void home(const State& _prev, const State& _next)
        {
          Command _command(gcodes_.home());
          if (_next.origin() == Origin::X)
          {
            _command('X',0);
          }
          insert(_command);
        }

        void temperature(const State& _prev, const State& _next)
        {
          if (_prev.extruder_temperature() != _next.extruder_temperature())
          {
            Command _command(gcodes_.set_temperature());
            _command('S',_next.extruder_temperature());
            insert(_command);
          }
        }

        void extrusion_positioning(const State& _prev, const State& _next)
        {
          if (_prev.extrusion_positioning() == Positioning::ABSOLUTE)
          {
            insert(Command(gcodes_.extrusion_abs_positioning()));
          }
        }

        void motors(const State& _prev, const State& _next)
        {
          if (_prev.motors() != _next.motors() && !_next.motors())
          {
            insert(Command(gcodes_.disable_motors()));
          }
        }

        void unit(const State& _prev, const State& _next)
        {
          if (_prev.unit() == LengthUnit::MILLIMETER)
          {
            insert(Command(gcodes_.unit_millimeter()));
          }
          else if (_next.unit() == LengthUnit::INCH)
          {
            insert(Command(gcodes_.unit_inch()));
          }
        }

        void set_position(float _value)
        {
          Command _command(gcodes_.set_position());
          _command('E',_value);
          insert(_command);
        }

        void controlled_movement(const Speed& _speed, const float& _extrusionPos)
        {
          Command _command(gcodes_.controlled_movement());
          _command('F',_speed.toMMmin());
          _command('E',_extrusionPos);
          insert(_command);
        }
        
        void controlled_movement(const Speed& _speed)
        {
          Command _command(gcodes_.controlled_movement());
          _command('F',_speed.toMMmin());
          insert(_command);
        }

        void controlled_movement(const State& _prev, const State& _next, const float& _extrusionPos)
        {
          Command _command(gcodes_.controlled_movement());

          if ((_prev.x() != _next.x()) || (_prev.y() != _next.y()))
          {
            /// Coordinates must be from next state!!!
            _command('X',_next.x())('Y',_next.y());
          } 
          if (_prev.z() != _next.z())
          {
            _command('Z',_next.z()); 
          }
          //if (_next.speed() != _prev.speed())
          //{
            _command('F',_prev.speed().toMMmin());
         // }
          if (!_prev.travel())
          {
            _command('E',_extrusionPos);
          } 



          if (_command.size() > 1)
            insert(_command);
        }

        void positioning(const State& _prev, const State& _next)
        {
          Command _command;
          if (_prev.positioning() == Positioning::ABSOLUTE)
          {
            _command(gcodes_.abs_positioning());
          }
          else if (_prev.positioning() == Positioning::RELATIVE)
          {
            _command(gcodes_.rel_positioning());
          }
          insert(_command);
        }

        void fan(const State& _prev, const State& _next) 
        {
          if (_prev.fan() != _next.fan())
          {
            Command _command;
            if (!_next.fan())
            {
              _command(gcodes_.fan_off());
            }
            else
            {
              _command(gcodes_.fan())('S',_next.fan());
            }
            insert(_command);
          }
        }

        friend std::ostream& operator<<(std::ostream& _os, const Generator& _gen)
        {
          for (auto& _command : _gen.commands_)
            _os << _command;
          return _os;
        }

      private:
        void insert(const Command& _command)
        {
          if (!_command.empty())
          {
            commands_.push_back(_command);
          }
        }
        std::vector<Command> commands_;
        GCodes const& gcodes_;
      };


      struct Interpreter
      {
        Interpreter(const GCodes& _gcodes) : 
          gcodes_(_gcodes) {}

        void operator()(const Command& _c, State& _state)
        {
          enum
          {
            NONE,
            CONTROLLED_MOVE,
            SET_POSITION,
            SET_TEMPERATURE
          } _commandState;
        
          State _lastState = _state;

          for (auto& _field : _c)
          {
            if (_field == gcodes_.home())
            {
              _state.origin(Origin::ALL);
              _commandState = NONE;
            }
            else if (_field == gcodes_.controlled_movement())
            {
              _state.origin(Origin::NONE);
              _commandState = CONTROLLED_MOVE;
            }
            else if (_field == gcodes_.set_position())
            {
              _commandState = SET_POSITION;
            }
            if (_field == gcodes_.set_temperature())
            {
              _commandState = SET_TEMPERATURE;
            }
            else if (_commandState == CONTROLLED_MOVE ||
                     _commandState == SET_POSITION)
            {
              switch(_field.letter())
              {
                case 'X':
                  _state.x() = _field.value();
                break;
                case 'Y':
                  _state.y() = _field.value();
                break;
                case 'Z':
                  _state.z() = _field.value();
                break;
                case 'F':
                  _state.speed() = _field.value();
                break;
                case 'E':
                  _state.extrusionWidth( _lastState.extruder_length() - _state.extruder_length());
                  _state.extruder_length() = _field.value();
              }
            }
            else if (_commandState == SET_TEMPERATURE)
            {
              if (_field.letter() == 'S')
              {
                _state.extruder_temperature() = _field.value();
              }
            }
            _lastState = _state;
          }
        } 
        void readFile(const std::string& _filename, Container& _gcode)
        {
          std::ifstream _if(_filename);
          Token _line;
          State _state;
          while(std::getline(_if, _line))
          {
            Command _c(_line);
            this->operator()(_c,_state);
            _gcode.push_back(_state,_c);
          }
        }
      
      private:
        const GCodes& gcodes_;
      };
    }   
  }
}
