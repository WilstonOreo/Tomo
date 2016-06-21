#pragma once

#include "Interface.hpp"
#include <tomo/tool/Tool.hpp>


namespace tomo
{
  namespace device
  {
    template<typename DERIVED>
    class Device :
      public Interface,
      public Serializer<DERIVED>,
      public Registrar<DERIVED>
    {
    public:
      TOMO_BASE_TYPES
      typedef DERIVED derived_type;

      Device(parent_type const* _parent) :
        Interface(_parent)
      {
        buildSources();
      }

      source_map_type sources() const
      {
        return sourceMap_;
      }

    private:
      std::map<id_type,SourceInterface*> sourceMap_;

      SourceInterface* getSource(const id_type& _id)
      {
        if (!sourceMap_.count(_id)) return nullptr;
        return sourceMap_[_id];
      }
      
      SourceInterface const* getSource(const id_type& _id) const
      {
        if (!sourceMap_.count(_id)) return nullptr;
        return sourceMap_.at(_id);
      }

      void buildSources()
      {
        tbd::detail::visit_each(derived(),base::detail::GetSources<DERIVED>(derived(),sourceMap_));
      }

      derived_type& derived()
      {
        return static_cast<derived_type&>(*this);
      }
    };
  }

  using device::Device;
}



