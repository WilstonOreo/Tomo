#pragma once

#include <tbd/boolean.h>
#include "Interface.hpp"
#include <tomo/device/Interface.hpp>
#include <tomo/item/Interface.hpp>
#include <tbd/config.h>

namespace tomo
{
  namespace tool
  {
    /// Util class to determine the tools in a toolset
    struct Library
    {
      TOMO_BASE_TYPES

      /// Returns all tools in a specific toolset
      static ElementIdSet tools(const Id& _toolset)
      {
        return getFromToolset(tools(),_toolset);
      }

      /// Returns all items in a specific toolset
      static ElementIdSet items(const Id& _toolset)
      {
        return getFromToolset(items(),_toolset);
      }

      /// Returns all devices in a specific toolset
      static ElementIdSet devices(const Id& _toolset)
      {
        return getFromToolset(devices(),_toolset);
      }

      static IdSet const& toolsets()
      {
        static IdSet toolsets_;
        if (toolsets_.empty())
        {
          toolsets_ = getToolsets(all());
        }
        return toolsets_;
      }

      /// Returns all tools
      static ElementIdSet const& tools()
      {
        static ElementIdSet tools_;
        if (tools_.empty())
        {
          tools_ = getFromRegistry<tomo::tool::Registry>();
        }
        return tools_;
      }

      /// Returns all items
      static ElementIdSet const& items()
      {
        static ElementIdSet items_;
        if (items_.empty())
        {
          items_ = getFromRegistry<tomo::item::Registry>();
        }
        return items_;
      }

      /// Returns all devices
      static ElementIdSet const& devices()
      {
        static ElementIdSet devices_;
        if (devices_.empty())
        {
          devices_ = getFromRegistry<tomo::device::Registry>();
        }
        return devices_;
      }

      /// Returns items, devices and tools
      static ElementIdSet all()
      {
        using tbd::boolean::operator|;
        return tools() | items() | devices();
      }

      static ElementIdSet match(std::string const& _str, bool _caseSensitive = false)
      {
        auto _ids = all();
        ElementIdSet _out;

        std::string _s = _caseSensitive ? _str : boost::to_lower_copy(_str);
        for (auto& _id : _ids)
        {
          auto _idStr = _id.str();
          if (_caseSensitive) boost::to_lower(_idStr);
          size_t _found = _idStr.find(_str);
          if (_found != std::string::npos)
          {
            _out.insert(_id);
          }
        }
        return _out;
      }

      /// Returns items, devices and tools from a specific toolset
      static ElementIdSet all(const Id& _toolset)
      {
        using tbd::boolean::operator|;
        return tools(_toolset) | items(_toolset) | devices(_toolset);
      }

      /// Prints items and tools
      friend std::ostream& operator<<(std::ostream& _os, const Library& _lib)
      {
        auto _ids = Library::all();
        for (auto _id : _ids)
        {
          _os << _id << std::endl;
        }
        return _os;
      }

      static void print()
      {
        for (auto& _toolset : toolsets())
        {
          std::cout << _toolset << std::endl;
          
          std::cout << "\tTools:" << std::endl;
          auto&& _tools = tools(_toolset);
          for (auto& _toolId : _tools)
          {
            std::cout << "\t\t" << _toolId.typeId() << std::endl;
            auto&& _tool = tomo::tool::Registry::create(_toolId,nullptr);
            printSourcesAndSinks(_tool); 
          }
          
          std::cout << "\tItems:" << std::endl;
          auto&& _items = items(_toolset);
          for (auto& _itemId : _items)
          {
            std::cout << "\t\t" << _itemId.typeId() << std::endl;
          }

          std::cout << "\tDevices:" << std::endl;
          auto&& _devices = devices(_toolset);
          for (auto& _deviceId : _devices)
          {
            std::cout << "\t\t" << _deviceId.typeId() << std::endl;
            auto&& _device = tomo::device::Registry::create(_deviceId,nullptr);
            printSources(_device); 
          }
        }
      }

      static tbd::Config& config()
      {
        static tbd::Config _config;
        return _config;
      }

    private:
      template<typename T>
      static void printSourcesAndSinks(T&& _t)
      {
        printSources(_t);
        printSinks(_t);
      }

      template<typename T>
      static void printSources(T&& _t)
      {
        auto _sources = _t->sources();
        if (_sources.empty()) return;
        std::cout << "\t\t Sources" << std::endl;
        printPorts(_sources);
      }
      
      template<typename T>
      static void printSinks(T&& _t)
      {
        auto _sinks = _t->sinks();
        if (_sinks.empty()) return;
        std::cout << "\t\t Sinks" << std::endl;
        printPorts(_sinks);
      }

      template<typename T>
      static void printPorts(const T& _ports)
      {
        for (auto& _port : _ports)
        {
          auto&& _types = _port.second->supportedTypes();
          std::string _typeStr;
          if (_types.empty())
          {
            _typeStr = "*";
          }
          else
          {
            for (auto& _type : _types)
            {
              _typeStr += _type.str() + ",";
            }
          }
          std::cout << "\t\t  " << _port.first << ":" << _typeStr << std::endl;
        }
      }

      static IdSet getToolsets(const ElementIdSet& _ids)
      {
        IdSet _toolsets;
for (auto& _id : _ids)
        {
          _toolsets.insert(_id.toolset());
        }
        return _toolsets;
      }

      template<typename REGISTRY>
      static ElementIdSet getFromRegistry()
      {
        ElementIdSet _ids;
for (auto& _class : REGISTRY::classes())
        {
          _ids.insert(_class.first);
        }
        return _ids;
      }

      static ElementIdSet getFromToolset(const ElementIdSet& _ids, const Id& _toolsetId)
      {
        if (!toolsets().count(_toolsetId)) return ElementIdSet();
        ElementIdSet _out;
        for (auto& _id : _ids)
        {
          if (_id.toolset() == _toolsetId)
            _out.insert(_id);
        }
        return _out;
      }

    };
  }

  using tool::Library;
}


