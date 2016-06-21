#pragma once

#include <tbd/boolean.h>

namespace tomo
{
  namespace item
  {
    namespace detail
    {
      template<typename...ITEMS>
      struct SourceHandler {};

      template<typename ITEM>
      struct SourceHandler<ITEM>
      {
        template<typename T, typename PTR>
        static bool is(PTR _ptr)
        {
          if (!_ptr) return false;
          return T::typeId() == _ptr->elementId();
        }

        static ElementIdSet elementIds()
        {
          return ElementIdSet({ITEM::typeId()});
        }

        template<typename PTR>
        static bool is_any_of(PTR _ptr)
        {
          if (!_ptr) return false;
          return is<ITEM>(_ptr);
        }
        
        template<typename T, typename SET>
        static bool unique(const SET& _sources)
        {
          for (auto& _source : _sources)
          {
            if (!_source.template is<T>())
              return false;
          }
          return true;
        }

        template<typename T, typename PTR>
        static T const* as(PTR _ptr) 
        {
          return is<T>(_ptr) ? static_cast<T const*>(_ptr) : nullptr;
        }
      };

      template<typename ITEM, typename...ITEMS>
      struct SourceHandler<ITEM,ITEMS...> :
        private SourceHandler<ITEM>,
        private SourceHandler<ITEMS...>
      {
        typedef SourceHandler<ITEM> head_type;
        typedef SourceHandler<ITEMS...> tail_type;

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
          if (head_type::template unique<T>(_items)) return true; 
          return tail_type::template unique<T>(_items);
        }
         
        static ElementIdSet elementIds()
        {
          using tbd::boolean::operator|;
          return head_type::elementIds() | tail_type::elementIds();
        }
      };

    }
  }
}
