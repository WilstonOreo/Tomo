#pragma once

#include "Interface.hpp"
#include "Sequence.hpp"
#include <tomo/device/Interface.hpp>
#include <tomo/item/Interface.hpp>
#include "Connection.hpp"
#include "Library.hpp"


namespace tomo
{
  namespace tool
  {
    class Chain :
      public tool::Interface,
      public Registrar<Chain>
    {
    public:
      TOMO_TOOL("generic",Toolchain)
      typedef device::Interface device_interface_type;
      typedef tool::Interface tool_interface_type;
      typedef item::Interface item_interface_type;

      Chain(parent_type const* _parent) :
        tool_interface_type(_parent) {}
      
      Chain(Library const* _library = nullptr, parent_type const* _parent = nullptr) :
        tool_interface_type(_parent),
        library_(_library) {}

      device_interface_type* addDevice(const ElementId& _elementId, const id_type& _id = id_type())
      {
        auto&& _uniqueId = makeUniqueId(_elementId,_id);
        ids_.insert(_uniqueId);
        return devices_.add(_elementId,_uniqueId,this);
      }

      device_interface_type* getDevice(const id_type& _id)
      {
        return devices_.get(_id);
      }

      device_interface_type const* getDevice(const id_type& _id) const
      {
        return devices_.get(_id);
      }

      bool removeDevice(device_interface_type* _device)
      {
        if (!_device) return false;
        auto _id = devices_.get(_device);
        TOMO_DBG(_id)
        connections_.disconnect(_device);
        ids_.erase(_id);
        return devices_.remove(_id);
      }

      bool removeDevice(const id_type& _id)
      {
        auto _device = devices_.get(_id);
        if (!_device) return false;
        connections_.disconnect(_device);
        ids_.erase(_id);
        return devices_.remove(_id);
      }

      item_interface_type* addItem(const ElementId& _elementId, const id_type& _id = id_type())
      {
        auto&& _uniqueId = makeUniqueId(_elementId,_id);
        TOMO_DBG(_uniqueId << " " << _id)
        ids_.insert(_uniqueId);
        return items_.add(_elementId,_uniqueId,this);
      }

      item_interface_type* getItem(const id_type& _id)
      {
        return items_.get(_id);
      }

      item_interface_type const* getItem(const id_type& _id) const
      {
        return items_.get(_id);
      }

      bool removeItem(item_interface_type* _item)
      {
        return removeItem(items_.get(_item));
      }

      bool removeItem(const id_type& _id)
      {
        auto _item = items_.get(_id);
        if (!_item) return false;
        ids_.erase(_id);
        connections_.disconnect(_item);
        return items_.remove(_id);
      }

      Chain* appendToolchain(const id_type& _id = id_type())
      {
        auto&& _uniqueId = makeUniqueId(typeId(),_id);
        ids_.insert(_uniqueId);
        return static_cast<Chain*>(tools_.append<Chain>(_uniqueId,library_,this));
      }

      Chain* insertToolchain(
          size_t _index,
          const id_type& _id = id_type())
      {
        auto&& _uniqueId = makeUniqueId(typeId(),_id);
        ids_.insert(_uniqueId);
        return static_cast<Chain*>(tools_.insert<Chain>(_index,_uniqueId,library_,this));
      }

      tool_interface_type* appendTool(
        const ElementId& _elementId,
        const id_type& _id = id_type())
      {
        auto&& _uniqueId = makeUniqueId(_elementId,_id);
        ids_.insert(_uniqueId);
        return tools_.append(_elementId,_uniqueId,this);
      }

      tool_interface_type* insertTool(
        size_t _index,
        const ElementId& _elementId,
        const id_type& _id = id_type())
      {
        auto&& _uniqueId = makeUniqueId(_elementId,_id);
        ids_.insert(_uniqueId);
        return tools_.insert(_index,_elementId,_uniqueId,this);
      }

      tool_interface_type* getTool(const id_type& _id)
      {
        return tools_.get(_id);
      }

      tool_interface_type const* getTool(const id_type& _id) const
      {
        return tools_.get(_id);
      }
      
      id_type getToolId(tool_interface_type const* _tool) const
      {
        return tools_.getId(_tool);
      }

      bool removeTool(tool_interface_type* _tool)
      {
        return removeTool(tools_.getId(_tool));
      }

      bool removeTool(const id_type& _id)
      {
        auto _tool = tools_.get(_id);
        connections_.disconnect(_tool);
        if (tools_.remove(_tool))
        {
          ids_.erase(_id);
          return true;
        }
        return false;
      }

      /*
      SourceInterface const* source(const id_type& _id) const
      {
        if (!parent_) return nullptr;
      }

      SourceInterface* addSource(const id_type& _id, const id_type& _typeId)
      {
        if (!parent_) return nullptr;
      }

      bool removeSource(const id_type& _id)
      {
        if (!parent_) return false;
      }

      SinkInterface const* sink(const id_type& _id) const
      {
        if (!parent_) return nullptr;
      }

      item_interface_type* addSink(const id_type& _id, const id_type& _typeId)
      {
        if (!parent_) return nullptr;
        return sinks_.add(_id,_typeId);
      }

      bool removeSink(const id_type& _id)
      {
        if (!parent_ || !sinks_.exists(_id)) return false;
        return sinks_.remove(_id);
      }

      */
      void clear()
      {
        tools_.clear();
        connections_.clear();
        devices_.clear();
        items_.clear();
      }

      bool load(const ConfigPath& _path, const Config& _config)
      {
        clear();
        if (_config.exists(_path/"tools"))
        {
          tools_.load(_path/"tools",_config,this);
          for (auto& _tool : tools_)
            ids_.insert(getToolId(_tool.get()));
        }
        if (_config.exists(_path/"items"))
        {
          items_.load(_path/"items",_config,this);
          for (auto& _item : items_)
            ids_.insert(_item.first);
        }
        if (_config.exists(_path/"devices"))
        {
          devices_.load(_path/"devices",_config,this);
          for (auto& _device : devices_)
           ids_.insert(_device.first);
        }
        
        std::vector<std::string> _connIds = _config.get_array(_path/"connections",std::vector<std::string>());
        for (auto& _id : _connIds)
          connect(_id);
        return true;
      }

      bool loadFromFile(const std::string& _filename)
      {
        Config _config(_filename);
        return load("",_config);
      }
      
      void saveToFile(const std::string& _filename) const
      {
        Config _config;
        save("",_config);
        _config.save(_filename);
      }


      void save(const ConfigPath& _path, Config& _config) const
      {
        tools_.save(_path/"tools",_config);
        items_.save(_path/"items",_config);
        devices_.save(_path/"devices",_config);
        _config.put_array(_path/"connections",connections_.connections());
      }

      void process()
      {
        for (auto& _tool : tools_)
        {
          if (_tool->valid()) _tool->process();
        }
      }

      Connection const* connect(
        std::string const& _id)
      {
        if (_id.empty()) return nullptr;
       
        typedef std::vector<std::string> split_vector_type;

        split_vector_type _splitList;
        boost::split(  
            _splitList,
            _id,
            boost::is_any_of(Connection::connectionSeparator()),
            boost::token_compress_on);
        if (_splitList.size() != 2) return nullptr;

        auto& _sinkLink = _splitList[0];
        auto& _sourceLink = _splitList[1];

        return connections_.connect(
            _id,
            getSourceFromString(_sourceLink),
            getSinkFromString(_sinkLink));
      }

      Connection const* connect(
        tool_interface_type* _sinkTool, const id_type& _sinkId,
        tool_interface_type* _sourceTool, const id_type& _sourceId)
      {
        return connections_.connect(
            Connection::makeId(tools_.getId(_sinkTool),_sinkId,tools_.getId(_sourceTool),_sourceId),
            _sourceTool->getSource(_sourceId), 
            _sinkTool->getSink(_sinkId));
      }

      Connection const* connect(
        tool_interface_type* _sinkTool, const id_type& _sinkId,
        device_interface_type* _sourceDevice, const id_type& _sourceId)
      {
        return connections_.connect(
            Connection::makeId(tools_.getId(_sinkTool),_sinkId,devices_.get(_sourceDevice),_sourceId),
            _sourceDevice->getSource(_sourceId), 
            _sinkTool->getSink(_sinkId));
      }

      Connection const* connect(
          item_interface_type* _item,
          tool_interface_type* _sourceTool,
          const id_type& _sourceId)
      {
        return connections_.connect(
            Connection::makeId(items_.get(_item),"",tools_.getId(_sourceTool),_sourceId),
            _sourceTool->getSource(_sourceId),
            _item->getSink());
      }
      
      Connection const* connect(
          item_interface_type* _item,
          device_interface_type* _sourceDevice,
          const id_type& _sourceId)
      {
        return connections_.connect(
            Connection::makeId(items_.get(_item),"",devices_.get(_sourceDevice),_sourceId),
            _sourceDevice->getSource(_sourceId),
            _item->getSink());
      }

      template<typename TOOL_OR_DEVICE>
      Connection const* connect(
          const id_type& _itemId,
          TOOL_OR_DEVICE* _sinkElement,
          const id_type& _sinkId)
      {
        return connect(items_.get(_itemId),_sinkElement,_sinkId);
      }

      bool connected(SourceInterface const* _source) const
      {
        return connections_.connected(_source); 
      }
      
      bool connected(SinkInterface const* _sink) const
      {
        return connections_.connected(_sink); 
      }

      template<typename ELEMENT>
      void disconnect(ELEMENT* _element)
      {
        connections_.disconnect(_element);
      }

      void disconnect(SourceInterface* _source)
      {
        connections_.disconnect(_source); 
      }

      void disconnect(SinkInterface* _sink)
      {
        connections_.disconnect(_sink); 
      }

      size_t numberConnections() const
      {
        return connections_.size();
      }
 
      std::set<Connection const*> fetchConnections() const
      {
        std::set<Connection const*> _connections;
        for (auto& _connection : connections_.connections())
        {
          _connections.insert(&_connection);
        }
        return _connections;
      }

      
        std::set<SourceInterface const*> findMatchingSources(
          SinkInterface const* _sink,
          tool_interface_type const* _sinkTool = nullptr) const
      {
        int _id = getToolIdx(_sinkTool);
        std::set<SourceInterface const*> _out;
        size_t _idx = 0;

        for (auto& _tool : tools_)
        {
          if (_idx > _id)
          {
            auto _sources = _tool->sources();
            for (auto& _source : _sources)
            {
              if (Connection::typeIdMatch(_sink,_source.second))
                _out.insert(_source.second);
            }
          }
          ++_idx;
        } 
        for (auto& _device : devices_)
        {
          auto _sources = _device.second->sources();
          for (auto& _source : _sources)
          {
            if (Connection::typeIdMatch(_sink,_source.second))
              _out.insert(_source.second);
          }
        }

        return _out;
      }
      
      std::set<SinkInterface const*> findMatchingSinks(
          SourceInterface const* _source,
          tool_interface_type const* _sourceTool = nullptr) const
      {
        std::set<SinkInterface const*> _out;
        int _id = !_sourceTool ? tools_.size() : getToolIdx(_sourceTool);
        size_t _idx = 0;
        for (auto& _tool : tools_)
        {
          if (_idx < _id)
          {
            auto _sinks = _tool->sinks();
            for (auto& _sink : _sinks)
            {
              if (Connection::typeIdMatch(_sink.second,_source))
                _out.insert(_sink.second);
            }
          } else { break; }
          ++_idx;
        } 
        for (auto& _item : items_)
        {
          if (Connection::typeIdMatch(_item.second->getSink(),_source))
            _out.insert(_item.second->getSink());
        }
        return _out;
      }

      source_map_type sources() const
      {
        source_map_type _sources;
        for (auto& _source : sources_)
        {
          _sources[_source.first] = const_cast<SourceInterface*>(static_cast<SourceInterface const*>(&_source.second));
        }
        return _sources;
      }

      sink_map_type sinks() const
      {
        sink_map_type _sinks;
        for (auto& _sink : sinks_)
        {
          _sinks[_sink.first] = const_cast<SinkInterface*>(static_cast<SinkInterface const*>(&_sink.second));
        }
        return _sinks;
      }

      Library const* library() const
      {
        return library_;
      }

      bool hasChildren() const
      {
        return true;
      }
      
      TBD_PROPERTY_REF_RO(Sequence,tools)
      TBD_PROPERTY_REF_RO(item::Storage,items)
      TBD_PROPERTY_REF_RO(device::Storage,devices)
    private:
      IdSet ids_;

      int getToolIdx(tool_interface_type const* _toolA) const
      {
        if (!_toolA) return -1;
        int _idx = 0;
        for (auto& _toolB : tools_)
        {
          if (_toolB.get() == _toolA) return _idx;
          ++_idx;
        }
        return -1;
      }

      SourceInterface* getSourceFromString(const std::string& _sourceLink)
      {
        typedef std::vector<std::string> split_vector_type;
        split_vector_type _sourceTokens;
        boost::split(  
            _sourceTokens,_sourceLink,
            boost::is_any_of(Connection::idSeparator()),
            boost::token_compress_on);
        if (_sourceTokens.size() != 2) return nullptr;
        SourceInterface* _source = nullptr;
        auto&& _tool = tools_.get(_sourceTokens[0]);
        auto&& _device = devices_.get(_sourceTokens[0]);
        if (_tool)
        {
          _source = _tool->getSource(_sourceTokens[1]); 
        } else
        if (_device)
        {
          _source = _device->getSource(_sourceTokens[1]); 
        }
        return _source;
      }

      SinkInterface* getSinkFromString(const std::string& _sinkLink)
      {
        typedef std::vector<std::string> split_vector_type;
        split_vector_type _sinkTokens;
        boost::split(  
            _sinkTokens,_sinkLink,
            boost::is_any_of(Connection::idSeparator()),
            boost::token_compress_on); 
        SinkInterface* _sink = nullptr;
        
        if (_sinkTokens.size() == 1)
        {
          auto&& _item = items_.get(_sinkTokens[0]);
          if (!_item) return nullptr;
          _sink = _item->getSink();
        } else
        if (_sinkTokens.size() == 2)
        {
          auto&& _tool = tools_.get(_sinkTokens[0]);
          if (!_tool) return nullptr;  
          _sink = _tool->getSink(_sinkTokens[1]); 
        }
        return _sink;
      }



      struct ConnectionContainer  
      {
        size_t size() const
        {
          return connections_.size();
        }

        void clear()
        {
          connections_.clear();
          sourceToConn_.clear();
          sinkToConn_.clear();
          sinkToSource_.clear();
          sourceToSink_.clear();
        }

        bool connected(SourceInterface const* _source) const
        {
          return sourceToConn_.count(_source) != 0;
        }
        
        bool connected(SinkInterface const* _sink) const
        {
          return sinkToConn_.count(_sink) != 0;
        }

        bool connectionPossible(SourceInterface const* _source, SinkInterface const* _sink) const
        {
          if (!_source || !_sink) return false;
          if (!_source->empty()) return false;
          if (connected(_source) || connected(_sink)) return false;
          if (!Connection::typeIdMatch(_sink,_source)) return false;
          return true;
        }

      Connection const* connect(
          const Connection::connection_id_type& _id,
          SourceInterface* _source, SinkInterface* _sink)
      {
        if (!connectionPossible(_source,_sink)) return nullptr;
        auto&& _conn = connections_.insert(Connection(_id,_sink,_source));
        auto _connPtr = &(*_conn.first); 
        _source->link(_sink->item());
        _sink->link(_source);
        sourceToConn_[_source] = _connPtr;
        sinkToConn_[_sink] = _connPtr;
        sinkToSource_[_sink] = _source;
        sourceToSink_[_source] = _sink;
        return _connPtr;
      }

      void disconnect(SourceInterface* _source)
      {
        if (!connected(_source)) return;

        auto&& _sink = sourceToSink_[_source];
        auto _conn = sourceToConn_[_source];
        _source->unlink();
        _sink->unlink();
        connections_.erase(*_conn);
        sourceToConn_.erase(_source);
        sinkToSource_.erase(_sink);
        sourceToSink_.erase(_source);
        sinkToConn_.erase(_sink);
      }
      
      void disconnect(SinkInterface* _sink)
      {
        if (!connected(_sink)) return;
        auto&& _source = sinkToSource_[_sink];
        auto&& _conn = sinkToConn_[_sink];
        _source->unlink();
        connections_.erase(*_conn);
        sourceToConn_.erase(_source);
        sinkToSource_.erase(_sink);
        sourceToSink_.erase(_source);
        sinkToConn_.erase(_sink);
      }

      void disconnect(tool::Interface* _tool)
      {
        disconnectPorts(_tool->sinks());
        disconnectPorts(_tool->sources());
      }
      
      void disconnect(device::Interface* _device)
      {
        disconnectPorts(_device->sources());
      }

      void disconnect(item::Interface* _item)
      {
        disconnect(_item->getSink());
      }

      TBD_PROPERTY_REF_RO(std::set<Connection>,connections)
      private:
        std::map<SourceInterface const*,Connection const*> sourceToConn_;
        std::map<SinkInterface const*,Connection const*> sinkToConn_;
        std::map<SourceInterface const*,SinkInterface*> sourceToSink_;
        std::map<SinkInterface const*,SourceInterface*> sinkToSource_;

        template<typename T> 
        void disconnectPorts(const T& _t)
        {
          for (auto& _port : _t)
          {
            disconnect(_port.second);
          }
        }
      } connections_;

      id_type makeUniqueId(const ElementId& _elementId, const id_type& _id)
      {
        auto&& _typeId = _elementId.typeId();
        if (!_id.valid())
        {
          return base::genId(_typeId,ids_);
        }
        if (ids_.count(_id))
        {
          return base::genId(_id,ids_);
        }
        return _id;
      }


      SourceInterface* getSource(const id_type& _id)
      {
        if (!sources_.count(_id)) return nullptr;
        return static_cast<SourceInterface*>(&sources_[_id]);
      }
      
      SourceInterface const* getSource(const id_type& _id) const
      {
        if (!sources_.count(_id)) return nullptr;
        return static_cast<SourceInterface const*>(&sources_.at(_id));
      }

      SinkInterface const* getSink(const id_type& _id) const
      {
        if (!sinks_.count(_id)) return nullptr;
        return static_cast<SinkInterface const*>(&sinks_.at(_id));
      }

      SinkInterface* getSink(const id_type& _id)
      {
        if (!sinks_.count(_id)) return nullptr;
        return static_cast<SinkInterface*>(&sinks_[_id]);
      }

      std::map<id_type,AnySource> sources_;
      std::map<id_type,AnySink> sinks_;
      Library const* library_;
    };
  }

  typedef tool::Chain Toolchain;
}

