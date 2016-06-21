#pragma once

#include "Interface.hpp"
#include "FileSource.hpp"

namespace tomo
{
  namespace item
  {
    struct NullLoader {};

    /// Item with FileSource
    template<typename DERIVED, typename LOADER = NullLoader>
    class Item :
      public Interface,
      public Serializer<DERIVED>,
      private Registrar<DERIVED>
    {
    public:
      TOMO_BASE_TYPES
      typedef DERIVED derived_type;
      typedef LOADER loader_type;

      Item(parent_type const* _parent) :
        Interface(_parent),
        fileSource_(static_cast<DERIVED&>(*this)),
        sink_(static_cast<Interface const*>(this))
      {}

      FileSourceInterface* fileSource()
      {
        return static_cast<FileSourceInterface*>(&fileSource_);
      }
      
      FileSourceInterface const* fileSource() const
      {
        return static_cast<FileSourceInterface const*>(&fileSource_);
      }

      virtual bool load(const ConfigPath& _path, const Config& _config)
      {
        bool _updated = Serializer<DERIVED>::load(_path,_config);
        if (_config.exists(_path/"file"))
        {          
          fileSource_.load(_config.get<std::string>(_path/"file"));
          _updated = true;
        }
        return _updated;
      }

      virtual void save(const ConfigPath& _path, Config& _config) const
      {
        Serializer<DERIVED>::save(_path,_config);
        _config.put(_path/"file",fileSource_.path());
      }

      SinkInterface const* getSink() const
      {
        return static_cast<SinkInterface const*>(&sink_);
      }
      
      SinkInterface* getSink()
      {
        return static_cast<SinkInterface*>(&sink_);
      }

    private:
      FileSource<DERIVED,LOADER> fileSource_;
      ItemSink sink_;
    };
    
    /// Item with FileSource
    template<typename DERIVED>
    class Item<DERIVED,NullLoader> :
      public Interface,
      public Serializer<DERIVED>,
      private Registrar<DERIVED>
    {
    public:
      TOMO_BASE_TYPES
      typedef DERIVED derived_type;

      Item(parent_type const* _parent) :
        Interface(_parent),
        sink_(static_cast<Interface const*>(this))
      {}

      FileSourceInterface* fileSource()
      {
        return nullptr;
      }
      
      FileSourceInterface const* fileSource() const
      {
        return nullptr;
      }

      SinkInterface const* getSink() const
      {
        return static_cast<SinkInterface const*>(&sink_);
      }
      
      SinkInterface* getSink()
      {
        return static_cast<SinkInterface*>(&sink_);
      }

    private:
      ItemSink sink_;
    };    
  }

  using item::Item;
}


