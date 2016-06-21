#pragma once 

#include "Source.hpp"
//#include "MultiSource.hpp"
#include "Sink.hpp"
//#include "MultiSink.hpp"

namespace tomo
{
  namespace item
  {
    namespace detail
    {
      struct None {};

      template<typename T>
      struct GetTypeId {};
     
      template<>
      struct GetTypeId<None>
      {
        TOMO_BASE_TYPES

        id_type operator()()
        {
          return "(none)";
        }
      };

      template<typename...ITEMS>
      struct GetTypeId<Source<ITEMS...>>
      {
        typedef typename Source<ITEMS...>::source_id_type id_type;

        template<typename T>
        id_type operator()(const T& _t)
        {
          if (!_t.valid()) return GetTypeId<None>()();
          return _t.get().getTypeId();
        }
      };

      template<typename...ITEMS> 
      struct GetTypeId<Sink<ITEMS...>> : 
        GetTypeId<Source<ITEMS...>> 
      {
        typedef typename Sink<ITEMS...>::sink_id_type id_type;
      };

      template<typename...ITEMS> 
      struct GetTypeId<MultiSource<ITEMS...>> 
      {
        typedef typename MultiSource<ITEMS...>::id_type id_type;
        
        template<typename T>
        id_type operator()(const T& _t)
        {
          auto _it = _t.begin();
          auto&& _typeId1 = GetTypeId<None>()();
          while (_it != _t.end())
          {
            _typeId1 = getTypeId(*_it); 
            ++_it;
            if (_it == _t.end()) break;
            auto&& _typeId2 = getTypeId(*_it);
            if (_typeId1 != _typeId2) return GetTypeId<None>()();
          }
          return _typeId1;
        }
      };

      template<typename...ITEMS>
      struct GetTypeId<MultiSink<ITEMS...>> : 
        GetTypeId<MultiSource<ITEMS...>>
      {
        typedef typename MultiSink<ITEMS...>::id_type id_type;
      };

      template<typename T>
      typename GetTypeId<T>::id_type getTypeId(const T& _t) 
      {
        return GetTypeId<T>()(_t);
      }
    }
  }
}
