#pragma once

#include "Chain.hpp"

namespace tomo
{
  namespace tool
  {
    namespace strategy
    {
      namespace for_each
      {
        struct Item {};
        struct Element {};
      }
    }

    namespace functor
    {
      template<typename T, typename STRATEGY>
      struct ForEach {};

      template<>
      struct ForEach<tool::Chain,strategy::for_each::Item>
      {
        template<typename T,typename STRATEGY, typename FUNCTOR>
        void operator()(const T& _t, STRATEGY, FUNCTOR _f)
        {
          for (auto& _idItem : _t.items())
          {
            auto _item = _idItem.second.get();
            _f(_item);
          }

          for (auto& _tool : _t.tools())
          {
            _f(_tool.get());
            auto&& _sinks = _tool->sinks();
            for (auto& _sink : _sinks)
            {
              if (_sink.second->empty()) continue;
              auto _item = _sink.second->item();
              _f(_item);
            }
          }

          for (auto& _idDevice : _t.devices())
          {
            auto _device = _idDevice.second.get();
            _f(_device);
          }

        }
      };
    }

    template<typename T, typename STRATEGY, typename FUNCTOR>
    void for_each(T& _t, STRATEGY _s, FUNCTOR _f)
    {
      functor::ForEach<T,STRATEGY>()(_t,_s,_f);
    }

    template<typename T, typename STRATEGY, typename FUNCTOR>
    void for_each(const T& _t, STRATEGY _s, FUNCTOR _f)
    {
      functor::ForEach<T,STRATEGY>()(_t,_s,_f);
    }
  }
}
