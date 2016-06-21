#pragma once

#include "SpecificSource.hpp"

namespace tomo
{
  namespace item
  {
    namespace detail
    {
      template<typename ITEM, typename...ITEMS>
      struct Source<ITEM,ITEMS...> : SourceInterface
      {
        TOMO_BASE_TYPES
        typedef detail::SourceHandler<ITEM,ITEMS...> handler_type;
        friend MultiSource<ITEM,ITEMS...>;

        Source(item_interface_type const* _item = nullptr) :
          item_(_item) {}

        bool empty() const
        {
          return item_ == nullptr;
        }

        operator bool() const
        {
          return !empty();
        }

        template<typename T>
        bool is() const
        {
          return handler_type::template is<T>(item_);
        }

        template<typename T>
        T const* as() const
        {
          return handler_type::template as<T>(item_);
        }

        ElementIdSet usedTypes(int _index = -1) const
        {
          return empty() ? ElementIdSet() : ElementIdSet({item_->elementId()});
        }

        ElementIdSet supportedTypes() const
        {
          return handler_type::elementIds();
        }

        item_interface_type const* item(int _index = -1) const
        {
          return item_;
        }

      private:
        void link(item_interface_type const* _item)
        {
          item_=_item;
        }

        void unlink(int _index = -1)
        {
          item_ = nullptr;
        }
        
        item_interface_type const* item_;
      };
    }

    template<typename ITEM, typename...ITEMS>
    using VariantSource = detail::Source<ITEM,ITEMS...>;
  }
}

