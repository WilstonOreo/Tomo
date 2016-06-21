#pragma once

#include "Sink.hpp"

namespace tomo
{
  namespace item
  {
    namespace detail
    {
      template<>
      struct Sink<> : SinkInterface
      {
        TOMO_BASE_TYPES

        Sink(parent_type const* _parent = nullptr) :
          parent_(_parent),
          item_(nullptr),
          source_(nullptr) 
        {
        }

        bool empty() const
        {
          return !(!item_) && (source_ == nullptr);
        }

        operator bool() const
        {
          return !empty();
        }

        template<typename T>
        bool is() const
        {
          return detail::SinkHandler<T>::is_any_of(item_.get());
        }

        template<typename T>
        T const* as() const
        {
          return detail::SinkHandler<T>::template as<T>(item_.get());
        }

        template<typename T>
        T * as()
        {
          return detail::SinkHandler<T>::template as_mutable<T>(item_.get());
        }

        template<typename T, typename...ARGS>
        T* make(ARGS&&..._args)
        {
          item_.reset(new T(parent_,_args...));
          return as<T>();
        }

        ElementIdSet usedTypes(int _index = -1) const
        {
          return empty() ? ElementIdSet() : ElementIdSet({item_->elementId()});
        }

        ElementIdSet supportedTypes() const
        {
          return ElementIdSet();
        }

        item_interface_type const* item(int _index = -1) const
        {
          return item_.get();
        }

        item_interface_type* item(int _index = -1) 
        {
          return item_.get();
        }

        parent_type const* parent() const
        {
          return parent_;
        }

      private:
        void link(SourceInterface const* _source) 
        {
          source_ = _source;
        }
        void unlink(int _index = -1) 
        {
          source_ = nullptr;
        }

        parent_type const* parent_;
        std::unique_ptr<item_interface_type> item_;
        SourceInterface const* source_;
      };
    }

    typedef detail::Sink<> AnySink;
  }
}
