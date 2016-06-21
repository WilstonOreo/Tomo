#pragma once

#include "Sink.hpp"

namespace tomo
{
  namespace item
  {
    namespace detail
    {
      /// A sink with one specificly typed item
      template<typename ITEM>
      struct Sink<ITEM> : public SinkInterface 
      {
        TOMO_BASE_TYPES
        typedef ITEM item_type;
        typedef id_type sink_id_type;
        typedef item_type* mutable_ptr_type;
        typedef item_type const* const_ptr_type;
        typedef item_type& mutable_ref_type;
        typedef item_type const& const_ref_type;
        friend MultiSink<ITEM>;

        Sink(parent_type const* _parent) :
          item_(_parent),
          source_(nullptr)
        {
        }

        bool empty() const
        {
          return source_ == nullptr;
        }
        
        operator bool() const
        {
          return !empty();
        }

        item_interface_type const* item(int _index = -1) const
        {
          return static_cast<item_interface_type const*>(&item_);
        }


        item_type* operator->()
        {
          return &item_;
        }

        item_type const* operator->() const
        {
          return &item_;
        }

        item_type& operator*()
        {
          return item_;
        }

        item_type const& operator*() const
        {
          return item_;
        }

        ElementIdSet usedTypes(int _index = -1) const
        {
          return supportedTypes();
        }

        ElementIdSet supportedTypes() const
        {
          return ElementIdSet({ITEM::typeId()});
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

        item_type item_;
        SourceInterface const* source_;
      };
    }

    template<typename ITEM>
    using SpecificSink = detail::Sink<ITEM>;
  }
}
