#pragma once

#include "Source.hpp"

namespace tomo
{
  namespace item
  {
    namespace detail
    {
      /// A source with an item of any type
      template<>
      struct Source<> : SourceInterface
      {
        TOMO_BASE_TYPES
        friend MultiSource<>;

        Source(item_interface_type const* _item = nullptr) :
          item_(_item)
        {}

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
          return detail::SourceHandler<T>::is_any_of(item_);
        }

        template<typename T>
        T const* as() const
        {
          return detail::SourceHandler<T>::template as<T>(item_);
        }

        item_interface_type const* item(int _index = -1) const
        {
          return item_;
        }

        ElementIdSet usedTypes(int _index = -1) const
        {
          return empty() ? ElementIdSet() : ElementIdSet({item_->elementId()});
        }

        ElementIdSet supportedTypes() const
        {
          return ElementIdSet();
        }

      private:
        void link(item_interface_type const* _item)
        {
          item_=_item;
        }
        void unlink(int _index = -1)
        {
          item_=nullptr;
        }

        item_interface_type const* item_;
      };
    }

    typedef detail::Source<> AnySource;
  }
}
