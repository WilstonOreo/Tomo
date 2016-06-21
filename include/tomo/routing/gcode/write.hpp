#pragma once

#include <fstream>
#include "Container.hpp"
#include "../iterate.hpp"

namespace tomo
{
  namespace routing
  {
    namespace gcode
    {
      template<typename STATE, typename COMMAND>
      void write(const Container<STATE,COMMAND>& _container, std::ostream& _os)
      {
        for (auto& _stateCommand : _container)
        {
          _os << _stateCommand.second;
        }
      }

      template<typename PATH, typename STATE, typename INCREMENTER, typename INTERPRETER>
      void write(const Route<PATH,STATE>& _route, std::ostream& _os, INTERPRETER _interp, INCREMENTER _inc)
      {
        auto _insert = [&_os](gcode::Command const& _cmd)
        {
          if (!_cmd.empty())
            _os << _cmd;
        };

        iterate(_route,[&](const STATE& _lastState, const STATE& _state)
        {
          _interp(_lastState,_state,_insert);
        },_inc);
      }      
    }
  }
}
