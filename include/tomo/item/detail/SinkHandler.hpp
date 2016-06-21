#pragma once

#include "SourceHandler.hpp"

namespace tomo
{
  namespace item
  {
    namespace detail
    {
      template<typename...ITEMS>
      struct SinkHandler {};

      template<typename ITEM>
      struct SinkHandler<ITEM> : SourceHandler<ITEM>
      {
        typedef SourceHandler<ITEM> base_type;

        using base_type::as;

        template<typename T, typename PTR> 
        static T* as_mutable(PTR _ptr)
        {
          return base_type::template is<T>(_ptr) ? static_cast<T*>(_ptr) : nullptr;
        }
      };

      template<typename ITEM, typename...ITEMS>
      struct SinkHandler<ITEM,ITEMS...> :
        private SinkHandler<ITEM>,
        private SinkHandler<ITEMS...>
      {
        TOMO_BASE_TYPES 
        typedef SinkHandler<ITEM> head_type;
        typedef SinkHandler<ITEMS...> tail_type;

        template<typename T, typename PTR>
        static T const* as(PTR _ptr) 
        {
          T const* _t = head_type::template as<T>(_ptr);
          if (!_t)
          {
            return tail_type::template as<T>(_ptr);
          }
          return _t;
        }

        template<typename T, typename PTR> 
        static T* as_mutable(PTR _ptr)
        {
          T* _t = head_type::template as_mutable<T>(_ptr);
          if (!_t)
          {
            return tail_type::template as_mutable<T>(_ptr);
          }
          return _t;
        }

        template<typename T, typename PTR>
        static bool is(PTR _ptr)
        {
          if (head_type::template is<T>(_ptr)) return true;
          return tail_type::template is<T>(_ptr);
        }
        
        template<typename PTR>
        static bool is_any_of(PTR _ptr)
        {
          if (head_type::is_any_of(_ptr)) return true;
          return tail_type::is_any_of(_ptr);
        }

        /// Tests if multiple items have a unique type
        template<typename T, typename SET>
        static bool unique(const SET& _items)
        {
          return head_type::template unique<T>(_items) || 
                 tail_type::template unique<T>(_items);
        }
      };
    }
  }
}
