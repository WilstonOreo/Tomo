#pragma once

#include "Source.hpp"

namespace tomo
{
  namespace item
  {
    namespace detail
    {
      /// A source with one specific typed item
      template<typename ITEM>
      struct Source<ITEM> :  SourceInterface
      {
        typedef ITEM item_type;
        typedef id_type source_id_type;
        typedef item_type const* item_ptr_type;
        typedef item_type const& item_ref_type;
        friend MultiSource<ITEM>;

        Source() : item_(nullptr)
        {
        }

        item_type const* operator->() const
        {
          return get();
        }

        item_type const& operator*() const
        {
          return *get();
        }

        bool empty() const
        {
          return item_ == nullptr;
        }

        operator bool() const
        {
          return !empty();
        }

        parent_type const* parent() const
        {
          if (empty()) return nullptr;
          return item_->parent();
        }

        item_type const* get() const
        {
          return item_;
        }

        item_interface_type const* item(int _index = -1) const
        {
          return static_cast<item_interface_type const*>(item_);
        }

        ElementIdSet usedTypes(int _index = -1) const
        {
          return empty() ? ElementIdSet() : supportedTypes();
        }

        ElementIdSet supportedTypes() const
        {
          return ElementIdSet({ITEM::typeId()});
        }

      private:
        void link(item_interface_type const* _item) 
        {
          item_ = static_cast<item_type const*>(_item);
        }
        void unlink(int _index = -1) 
        {
          item_ = nullptr;
        }

        item_type const* item_;
      };
    }

    template<typename ITEM>
    using SpecificSource = detail::Source<ITEM>;    
  }
}

