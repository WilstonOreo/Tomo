#pragma once

#include "SpecificSink.hpp"

namespace tomo
{
  namespace item
  {
    namespace detail
    {
      template<typename ITEM, typename...ITEMS>
      struct Sink<ITEM,ITEMS...> : SinkInterface
      {
        TOMO_BASE_TYPES
        typedef detail::SinkHandler<ITEM,ITEMS...> handler_type;
        friend MultiSink<ITEM,ITEMS...>;

        Sink(parent_type const* _parent) :
          parent_(_parent),
          item_(nullptr)
        {
        }

        bool empty() const
        {
          return !!item_; 
        }

        operator bool() const
        {
          return item();
        }

        parent_type const* parent() const
        {
          return parent_;
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

        template<typename T>
        T* as()
        {
          return handler_type::template as_mutable<T>(item_);
        }

        template<typename T, typename...ARGS>
        T* make(ARGS&&..._args)
        {
          if (supportedTypes().count(T::typeId()) == 0) return nullptr;
          item_.reset(new T(parent_,_args...));
          return as<T>();
        }
        
        item_interface_type const* item(int _index = -1) const
        {
          return item_.get();
        }

        item_interface_type* item(int _index = -1) 
        {
          return item_.get();
        }
        
        ElementIdSet usedTypes(int _index = -1) const
        {
          return empty() ? ElementIdSet() : ElementIdSet({item_->elementId()});
        }

        ElementIdSet supportedTypes() const
        {
          return handler_type::elementIds();
        }

      private:
        std::unique_ptr<item_interface_type> item_;
        parent_type const* parent_;
      };
    }

    template<typename ITEM, typename...ITEMS>
    using VariantSink = detail::Sink<ITEM,ITEMS...>;
  }
}
