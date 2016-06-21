#pragma once

#include "Sink.hpp"

namespace tomo
{
  namespace item
  {
    /// An Item Sink holds only a pointer to an item
    class ItemSink : public SinkInterface
    {
    public:
      TOMO_BASE_TYPES
      ItemSink(item_interface_type const* _item) :
        item_(_item),
        source_(nullptr) {}

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
        return item_;
      }

      ElementIdSet usedTypes(int _index = -1) const
      {
        return supportedTypes();
      }

      ElementIdSet supportedTypes() const
      {
        return ElementIdSet({item_->elementId()});
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

      item_interface_type const* item_;
      SourceInterface const* source_;
    };
  }
  using item::ItemSink;
}
