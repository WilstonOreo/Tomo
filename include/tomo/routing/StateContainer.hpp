#pragma once

#include <vector>
#include <algorithm>
#include <gex/base/Range.hpp>
#include "iterate.hpp"

namespace tomo
{
  namespace routing
  {
    using gex::base::Range;

    template<typename STATE>
    struct StateContainer : 
      private std::vector<STATE>
    {
      typedef STATE state_type;
      typedef std::vector<state_type> container_type;
      typedef typename container_type::const_iterator const_iterator;
      typedef typename state_type::time_type time_type;

      using container_type::operator[];
      using container_type::begin;
      using container_type::end;
      using container_type::clear;
      using container_type::empty;

      StateContainer() {}

      template<typename PATH>
      StateContainer(const PATH& _path)
      {
        generate(_path);
      }

      template<typename PATH>
      void generate(const PATH& _path)
      {
        container_type::reserve(_path.numberOfMovements());
        iterate(_path,[&](const STATE& _state)
        {
          container_type::push_back(_state);
        });
      }

      void sort()
      {
        std::sort(begin(),end(),[&](const STATE& _lhs, const STATE& _rhs) -> bool
        {
          return _lhs.time() < _rhs.time();
        });
      }

      std::pair<const_iterator,const_iterator> timeInterval(const Range<time_type>& _interval) const
      {
        auto _sort = [&](const state_type& _state, const time_type& _time) -> bool
        {
          return _state.time() < _time;
        };
        std::pair<const_iterator,const_iterator> _r;
        _r.first  = std::lower_bound(begin(),end(),_interval.min(),_sort);
        _r.second = std::upper_bound(begin(),end(),_interval.max(),_sort);
        return _r;
      }
    };
  }
}
