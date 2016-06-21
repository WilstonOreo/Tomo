#pragma once

#include <vector>
#include <utility>
#include "Command.hpp"
#include "tomo/base/types/time.hpp"

namespace tomo
{
  namespace routing
  {
    namespace gcode
    {
      template<typename STATE, typename COMMAND>
      struct Container : 
        private std::vector<std::pair<STATE,COMMAND>>
      {
        typedef STATE state_type;
        typedef COMMAND command_type;
        typedef std::pair<state_type,command_type> pair_type;
        typedef std::vector<pair_type> container_type;

        using typename container_type::value_type;
        using container_type::operator[];
        using container_type::begin;
        using container_type::back;
        using container_type::end;
        using container_type::clear;
        using container_type::empty;
        using container_type::size;
        typedef typename container_type::const_iterator const_iterator;
        
        Container() {}
 
        void sort()
        {
          std::sort(begin(),end(),
              [&](const pair_type& _lhs, const pair_type& _rhs) -> bool
          {
            return _lhs.first.time() < _rhs.first.time();
          });
        }

        void push_back(const state_type& _state, const Command& _c)
        {
          if (!empty()) 
          {
            auto& _back = container_type::back().first;
            if (_back.time() > _state.time()) return;
          }
          container_type::emplace_back(_state,_c);
        }
        
        void push_back(const pair_type& _pair)
        {
          push_back(_pair.first,_pair.second);
        }

        std::pair<const_iterator,const_iterator> period(const Period& _period) const
        {
          std::pair<const_iterator,const_iterator> _r;
          auto _sort = [&](const pair_type& _state, const Time& _time) -> bool
          {
            return _state.first.time() < _time;
          };
          _r.first = std::lower_bound(begin(),end(),_period.first,_sort);
          _r.second = std::upper_bound(begin(),end(),_period.second,_sort);
          return _r;
        }

        void print(std::ostream& _os) const
        {
          for (auto& _stateCommands : *this)
            _os << _stateCommands.second;
        }

        friend std::ostream& operator<<(std::ostream& _os, const Container& _gcode)
        {
          _gcode.print(_os);
          return _os;
        }
      };
    }
  }
}
