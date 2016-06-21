#pragma once

#include <tbd/serialize_unique_ptr.h>

/// Storage template class for storing items and devices
namespace tomo
{
  namespace base
  {
    /**@brief Container class for storing items
      *@detail Each item is accessible by a unique id
    **/
    template<typename REGISTRY>
    struct Storage :
      private std::map<Id,Ptr<typename REGISTRY::interface_type>>,
      private std::map<typename REGISTRY::interface_type const*,Id>
    {
      typedef typename REGISTRY::interface_type interface_type;
      typedef Ptr<interface_type> ptr_type;
      typedef std::map<Id,ptr_type> map_type;
      typedef std::map<interface_type const*,Id> inv_map_type;
      typedef typename map_type::const_iterator map_iterator_type;
      typedef Id id_type;

      using map_type::empty;
      using map_type::begin;
      using map_type::end;
      using map_type::size;

      void clear()
      {
        map_type::clear();
        inv_map_type::clear();
      }

      /// Add an item from a given typeId and returns its interface if typeId exists
      template<typename...ARGS>
      interface_type* add(
        const ElementId& _typeId,
        const id_type& _id,
        ARGS&&..._args)
      {
        if (exists(_id)) return nullptr;
        auto&& _item = REGISTRY::create(_typeId,_args...);
        if (!_item) return nullptr;
        map_type::operator[](_id) = std::move(_item);
        inv_map_type::operator[](map_type::at(_id).get()) = _id;
        return map_type::at(_id).get();
      }

      bool remove(id_type const& _id)
      {
        if (!map_type::count(_id)) return false;
        interface_type const* _itemPtr = map_type::operator[](_id).get();
        return  
          (inv_map_type::erase(_itemPtr)) &&
          (map_type::erase(_id));
      }

      /// Checks if item with id exists
      inline bool exists(const id_type& _id) const
      {
        return map_type::count(_id) != 0 && _id.valid();
      }
      
      /// Checks if item with id exists
      inline bool exists(interface_type const* _item) const
      {
        return inv_map_type::count(_item) != 0;
      }

      /// Get item interface from a certain id (mutable version)
      inline interface_type* get(const id_type& _id)
      {
        if (!exists(_id)) return nullptr;
        return map_type::operator[](_id).get();
      }

      /// Get item interface from a certain id (const version)
      inline interface_type const* get(const id_type& _id) const
      {
        if (!exists(_id)) return nullptr;
        return map_type::at(_id).get();
      }
      
      /// Get item interface from a certain id (mutable version)
      inline Id get(interface_type const* _item)
      {
        if (!exists(_item)) return Id();
        return inv_map_type::operator[](_item);
      }

      /// Get item interface from a certain id (const version)
      inline Id get(interface_type const* _item) const
      {
        if (!exists(_item)) return Id();
        return inv_map_type::at(_item);
      }

      /*
          /// Retrieve all item ids
          inline Interface::idset_type getIds() const
          {
            Interface::idset_type _ids;
            for (auto& _p : *this)
              _ids.insert(_p.first);
            return _ids;
          }
      */
      template<typename...ARGS>
      bool load(const ConfigPath& _path, const Config& _config, const ARGS&..._args)
      {
        clear();
        auto& _tree = _config.get_child(_path);

        for (auto& _idItem : _tree)
        {
          auto _cfgPath = _path/ConfigPath(_idItem.first);
          auto& _id = _idItem.first;

          ptr_type _item =
            tbd::SerializeRegistered<REGISTRY>::load(_cfgPath,_config,_args...);
          if (_item)
          {
            map_type::operator[](_id) = std::move(_item);
            inv_map_type::operator[](map_type::at(_id).get()) = _id;
          }
        }
        return true;
      }

      void save(const ConfigPath& _path, Config& _config) const
      {
        for (auto& _idItem : *this)
        {
          auto _cfgPath = _path/ConfigPath(_idItem.first.str());
          tbd::SerializeRegistered<REGISTRY>::save(_idItem.second,_idItem.second->elementId().str(),_cfgPath,_config);
        }
      }

    };
  }

}


