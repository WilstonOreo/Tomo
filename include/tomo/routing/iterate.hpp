#pragma once

#include "Path.hpp"
#include "Route.hpp"
#include "gcode/Container.hpp"
#include <tbd/add_const_if.h>

namespace tomo
{
  namespace routing
  {
    namespace strategy
    {
      struct Default {};
      struct State {};

      template<typename INCREMENTER>
      struct StateWithIncrementer 
      {
        typedef INCREMENTER incrementer_type;

        StateWithIncrementer(incrementer_type _incrementer) :
          incrementer_(_incrementer) {}

        TBD_PROPERTY(incrementer_type,incrementer)
      };
    }

    namespace functor
    {
      template<typename ITERABLE, typename STRATEGY>
      struct Iterate {};
    }

    template<typename ITERABLE>
    struct IterateType {};

    template<typename MOVE>
    struct IterateType<Path<MOVE>>
    {
      typedef MOVE type;
    };

    template<typename PATH, typename STATE>
    struct IterateType<Route<PATH,STATE>>
    {
      typedef STATE type;
    };

    template<typename STATE, typename COMMAND>
    struct IterateType<gcode::Container<STATE,COMMAND>>
    {
      typedef typename gcode::Container<STATE,COMMAND>::pair_type type;
    };

    using functor::Iterate;

    template<typename ITERABLE, typename FUNCTOR>
    void iterate(const ITERABLE& _p, FUNCTOR f)
    {
      Iterate<ITERABLE,strategy::Default>()(_p,strategy::Default(),f);
    }
    
    template<typename ITERABLE, typename STRATEGY, typename FUNCTOR>
    void iterate(ITERABLE& _p, STRATEGY _s, FUNCTOR f)
    {
      Iterate<ITERABLE,STRATEGY>()(_p,_s,f);
    }
    
    template<typename ITERABLE, typename STRATEGY, typename FUNCTOR>
    void iterate(const ITERABLE& _p, STRATEGY _s, FUNCTOR f)
    {
      Iterate<ITERABLE,STRATEGY>()(_p,_s,f);
    }

    namespace functor
    {
      template<typename MOVE>
      struct Iterate<Path<MOVE>,strategy::Default>
      {
        template<typename STRATEGY, typename FUNCTOR>
        void operator()(const Path<MOVE>& _path, STRATEGY, FUNCTOR _f)
        {
          gex::for_each<MOVE>(_path,_f);
        }
      };
 
      template<typename PATH, typename STATE>
      struct Iterate<Route<PATH,STATE>,strategy::Default>
      {
        template<typename ROUTE, typename STRATEGY, typename FUNCTOR>
        void operator()(const ROUTE& _route, STRATEGY _s, FUNCTOR _f)
        {
          for (size_t i = 0; i < _route.size(); ++i)
          {
            _f(_route.path()[i],_route.events(i));
          }
        }

        template<typename ROUTE, typename STRATEGY, typename FUNCTOR>
        void operator()(ROUTE& _route, STRATEGY _s, FUNCTOR _f)
        {
          for (size_t i = 0; i < _route.size(); ++i)
          {
            _f(_route.path()[i],_route.events(i));
          }
        }
      };        

      template<typename PATH, typename STATE, typename INCREMENTER>
      struct Iterate<Route<PATH,STATE>,strategy::StateWithIncrementer<INCREMENTER>>
      {
        template<typename ROUTE, typename STRATEGY, typename FUNCTOR>
        void operator()(const ROUTE& _route, STRATEGY _s, FUNCTOR _f)
        {
          iterate<true,ROUTE>(_route,_f,_s.incrementer());
        }
        
        template<typename ROUTE, typename STRATEGY, typename FUNCTOR>
        void operator()(ROUTE& _route, STRATEGY _s, FUNCTOR _f)
        {
          iterate<false,ROUTE>(_route,_f,_s.incrementer());
        }
      private:
        template<bool IS_CONST, typename ROUTE, typename FUNCTOR>
        void iterate(typename tbd::AddConstIf<ROUTE,IS_CONST>::ref_type _route, FUNCTOR _f, INCREMENTER _inc)
        {
          STATE _state = _route.initial();
          STATE _lastState = _state; 
          typedef typename Route<PATH,STATE>::eventid_type eventid_type;
          eventid_type _pos = 0;

          auto newState = [&]()
          {
            _inc(_lastState,_state);
            _f(_lastState,_state);
            _lastState = _state;
          };

          for (auto& _move : _route.path())
          {
            _state(_move);
            if (_route.events().count(_pos) > 0)
            {
              auto& _events = _route.events().at(_pos);
              for (auto& _event : _events)
              {
                _event->invoke(_state);
              }
            }
            newState();
            ++_pos;
          }

          auto _begin = _route.events().lower_bound(_pos);
          auto _end = _route.events().end();
          for (auto it = _begin; it != _end; ++it)
          {
            auto& _events = it->second;
            for (auto& _event : _events)
            {
              _event->invoke(_state);
            }
            newState();
          }
        }
      };

      template<typename PATH, typename STATE>
      struct Iterate<Route<PATH,STATE>,strategy::State>
      {
        template<typename ROUTE, typename STRATEGY, typename FUNCTOR>
        void operator()(const ROUTE& _route, STRATEGY, FUNCTOR _f)
        {
          typedef std::function<void(const STATE&, const STATE&)> incrementer_type;
          iterate(_route,strategy::StateWithIncrementer<incrementer_type>([](const STATE& _prev, const STATE& _next){}),_f);
        }
      };

      template<typename STATE, typename COMMAND>
      struct Iterate<gcode::Container<STATE,COMMAND>,strategy::Default>
      {
        template<typename STRATEGY, typename FUNCTOR>
        void operator()(const gcode::Container<STATE,COMMAND>& _gcode, STRATEGY, FUNCTOR _f)
        {
          for (auto& _state_command_pair : _gcode)
          {
            _f(_state_command_pair);
          }
        }
      };
    }
  }
}

