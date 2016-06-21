#pragma once

#include <tbd/boolean.h>
#include <tomo/base/Element.hpp>
#include <tomo/item/SourceAndSink.hpp>

namespace tomo
{
  namespace tool
  {
      struct Connection
      {
        TOMO_BASE_TYPES
        typedef std::string connection_id_type;

        Connection(const connection_id_type& _id,
            SinkInterface const* _sink,
            SourceInterface const* _source) :
          id_(_id),
          sink_(_sink),
          source_(_source) {}
        
        friend bool operator<(const Connection& _lhs, const Connection& _rhs)
        {
          return _lhs.id_ < _rhs.id_;
        }

        static connection_id_type makeId(
          const id_type& _sinkElementId,
          const id_type& _sinkId,
          const id_type& _sourceElementId,
          const id_type& _sourceId)
        {
          if (_sinkElementId.empty() || _sourceElementId.empty()) return connection_id_type();
          std::string _sink = (_sinkId.empty()) ? std::string(_sinkElementId) : 
            std::string(_sinkElementId)+
            idSeparator() +
            std::string(_sinkId);
          connection_id_type _id = _sink+
            connectionSeparator()+
            std::string(_sourceElementId)+
            idSeparator()+ 
            std::string(_sourceId);

          TOMO_DBG(_id)
          return _id;
        }

        static bool typeIdMatch(
            SinkInterface const* _sink,
            SourceInterface const* _source)
        {
          using tbd::boolean::operator&;
          if (_source->supportedTypes().empty()) return true;
          auto&& _intersection = _sink->usedTypes() & _source->supportedTypes();
          return !_intersection.empty();
        }

        friend std::ostream& operator<<(std::ostream& _os, Connection const& _connection)
        {
          _os << _connection.id_;
          return _os;
        }

        bool valid() const
        {
          return sink() && source() && !id().empty();
        }

        static std::string connectionSeparator()
        {
          return "~";
        }
        
        static std::string idSeparator()
        {
          return ".";
        }

        TBD_PROPERTY_REF_RO(connection_id_type,id)
        TBD_PROPERTY_RO(SinkInterface const*,sink)
        TBD_PROPERTY_RO(SourceInterface const*,source)
      };

  }
}
