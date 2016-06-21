#pragma once

#include <list>
#include "SingleSource.hpp"

namespace tomo
{
  namespace item
  {
    namespace detail
    {
      template<typename...ITEMS>
      struct MultiSource :
        private std::list<SingleSource<ITEMS...>>
      {
        TOMO_BASE_TYPES
        typedef SingleSource<ITEMS...> source_type;
        typedef detail::SourceHandler<ITEMS...> handler_type;
        friend source_type;
        typedef std::list<source_type> container_type;

        using container_type::empty;
        using container_type::begin;
        using container_type::end;

        MultiSource(mutable_parent_type _parent, const idset_type& _ids) :
          parent_(_parent)
        {
          make(_ids);
        }

        MultiSource(mutable_parent_type _parent) :
          parent_(_parent)
        {}

        template<typename T>
        bool is() const
        {
          return handler_type::template unique<T>(*this);
        }

        bool valid() const
        {
          return parent() && allValid();
        }

        operator bool() const
        {
          return valid();
        }

        friend std::ostream& operator<<(std::ostream& _os, const MultiSource& _src)
        {
          _os << _src.ids();
          return _os;
        }

        friend std::istream& operator>>(std::istream& _is, MultiSource& _src)
        {
          idset_type _ids;
          _is >> _ids;
          _src.make(_ids);
          return _is;
        }

        const_parent_type parent() const
        {
          return parent_;
        }

        mutable_parent_type parent()
        {
          return parent_;
        }

      private:
        void make(const idset_type& _ids)
        {
          container_type::clear();
          for (auto& _id : _ids)
          {
            container_type::emplace_back(parent_,_id);
          }
        }

        idset_type ids() const
        {
          idset_type _ids;
for (auto& _source : *this)
            _ids.insert(_source.id_);
          return _ids;
        }

        bool allValid() const
        {
for (auto& _source : *this)
          {
            if (!_source.valid()) return false;
          }
          return true;
        }

        mutable_parent_type parent_;
      };
    }
  }
}
