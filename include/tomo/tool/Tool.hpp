#pragma once

#include "Interface.hpp"

namespace tomo
{
  namespace base
  {
    namespace detail
    {
      template<typename T>
      struct GetSourcesAndSinks
      {
        TOMO_BASE_TYPES
        typedef std::map<id_type,SourceInterface*> source_map_type;
        typedef std::map<id_type,SinkInterface*> sink_map_type;

        GetSourcesAndSinks(
          T& _obj,
          source_map_type& _sourceMap,
          sink_map_type& _sinkMap) :
          obj_(_obj),
          sourceMap_(_sourceMap),
          sinkMap_(_sinkMap) {}

        template<typename F>
        void operator()(const F& _f)
        {
          if (id_type(_f.typeId()) == SourceInterface::typeId())
          {
            sourceMap_[id_type(_f.name())] = static_cast<SourceInterface*>(_f.get_void(obj_));
          }
          else if (id_type(_f.typeId()) == SinkInterface::typeId())
          {
            sinkMap_[id_type(_f.name())] = static_cast<SinkInterface*>(_f.get_void(obj_));
          }
        }

      private:
        T& obj_;
        source_map_type& sourceMap_;
        sink_map_type& sinkMap_;
      };

      template<typename T>
      struct GetSources
      {
        TOMO_BASE_TYPES
        typedef std::map<id_type,SourceInterface*> source_map_type;

        GetSources(
          T& _obj,
          source_map_type& _sourceMap) :
          obj_(_obj),
          sourceMap_(_sourceMap) {}

        template<typename F>
        void operator()(const F& _f)
        {
          if (id_type(_f.typeId()) == SourceInterface::typeId())
          {
            sourceMap_[id_type(_f.name())] = static_cast<SourceInterface*>(_f.get_void(obj_));
          }
        }

      private:
        T& obj_;
        source_map_type& sourceMap_;
      };
    }
  }



  namespace tool
  {
    template<typename DERIVED>
    class Tool :
      public Interface,
      public Serializer<DERIVED>,
      private Registrar<DERIVED>
    {
    public:
      TOMO_BASE_TYPES
      typedef DERIVED derived_type;

      Tool(parent_type const* _parent) :
        Interface(_parent)
      {
        buildSourcesAndSinks();
      }

      source_map_type sources() const
      {
        return sourceMap_;
      }

      sink_map_type sinks() const
      {
        return sinkMap_;
      }

    private:
      source_map_type sourceMap_;
      sink_map_type sinkMap_;

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

      SinkInterface const* getSink(const id_type& _id) const
      {
        if (!sinkMap_.count(_id)) return nullptr;
        return sinkMap_.at(_id);
      }

      SinkInterface* getSink(const id_type& _id)
      {
        if (!sinkMap_.count(_id)) return nullptr;
        return sinkMap_[_id];
      }

      void buildSourcesAndSinks()
      {
        tbd::detail::visit_each(derived(),base::detail::GetSourcesAndSinks<DERIVED>(derived(),sourceMap_,sinkMap_));
      }


      derived_type& derived()
      {
        return static_cast<derived_type&>(*this);
      }
      derived_type const& derived_const() const
      {
        return static_cast<derived_type const&>(*this);
      }
    };
  }

  using tool::Tool;
}


