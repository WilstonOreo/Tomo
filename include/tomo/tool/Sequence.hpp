#pragma once

#include <list>
#include "Interface.hpp"

namespace tomo
{
  namespace tool
  {
    struct Sequence;
  }
}

namespace tbd
{
  namespace detail
  {
    template<>
    struct ParameterTypeInfo<tomo::tool::Chain>
    {
      typedef tomo::tool::Registry registry_type;

      template<typename PTR>
      tomo::ElementId operator()(const PTR& _ptr)
      {
        return _ptr->elementId();
      }
    };

    template<>
    struct ParameterTypeInfo<tomo::tool::Sequence> :
        ParameterTypeInfo<tomo::tool::Chain>
    {
      typedef tomo::tool::Registry registry_type;
    };
  }
}




namespace tomo
{
  namespace tool
  {
    /// A Sequence is a list of tools which can be access by a unique id
    struct Sequence :
      private std::list<Ptr<Interface>>
    {
    private:
      typedef Ptr<Interface> ptr_type;
      typedef std::list<ptr_type> list_type;
      typedef Interface::id_type id_type;

      std::map<id_type,typename list_type::iterator> ids_;
      std::map<Interface const*,id_type> ptrs_;

    public:
      using list_type::const_iterator;
      using list_type::iterator;
      using list_type::begin;
      using list_type::end;
      using list_type::empty;
      using list_type::size;

      void clear()
      {
        list_type::clear();
        ids_.clear();
        ptrs_.clear();
      }

      template<typename...ARGS>
      bool load(const ConfigPath& _path, const Config& _config, ARGS&&..._args)
      {
        clear();
        size_t _number = _config.get<size_t>(_path / "number");
        for (size_t _idx = 0; _idx < _number; ++_idx)
        {
          auto _cfgPath = _path/ConfigPath(std::to_string(_idx));
          auto _id = _config.get<id_type>(_cfgPath/"id");

          ptr_type _tool =
            tbd::SerializeRegistered<Registry>::load(_cfgPath,_config,_args...);
          if (_tool)
          {
            list_type::push_back(std::move(_tool));
            ptrs_.insert( {list_type::back().get(),_id});
            ids_.insert( {_id,--list_type::end()});
          }
        }
        return true;
      }

      void save(const ConfigPath& _path, Config& _config) const
      {
        size_t _idx = 0;
        _config.put(_path / "number",size());

for (auto& _ptr : ptrs_)
        {
          std::cout << _ptr.first << "\t" << _ptr.second << std::endl;
        }


for (auto& _tool : *this)
        {
          auto _cfgPath = _path/ConfigPath(std::to_string(_idx));
          tbd::SerializeRegistered<tool::Registry>::save(_tool,std::string(_tool->elementId()),_cfgPath,_config);
          _config.put(_cfgPath/"id",ptrs_.at(_tool.get()));
          ++_idx;
        }
      }
      
      Interface::id_type getId(Interface const* _tool) const
      {
        if (!ptrs_.count(_tool)) return Interface::id_type();
        return ptrs_.at(_tool);
      }

      Interface* get(const id_type& _id)
      {
        if (!ids_.count(_id)) return nullptr;
        return ids_[_id]->get();
      }

      Interface const* get(const id_type& _id) const
      {
        if (!ids_.count(_id)) return nullptr;
        return ids_.at(_id)->get();
      }


      template<typename...ARGS>
      Interface* insert(size_t _index,
                        const ElementId& _elementId,
                        const id_type& _id,
                        ARGS&&..._args)
      {
        if (get(_id) || (_index > size())) return nullptr;
        list_type::iterator it = begin();
        std::advance(it, _index);
        auto&& _tool = Registry::create(_elementId,_args...);
        if (!_tool) return nullptr;
        return insert(_tool,_id,it);
      }

      template<typename TYPE, typename...ARGS>
      Interface* insert(size_t _index,
                        const id_type& _id,
                        ARGS&&..._args)
      {
        if (get(_id) || (_index > size())) return nullptr;
        ptr_type _tool(new TYPE(_args...));
        if (!_tool) return nullptr;
        list_type::iterator it = begin();
        std::advance(it, _index);
        return insert(_tool,_id,it);
      }


      template<typename...ARGS>
      Interface* append(
        const ElementId& _elementId,
        const id_type& _id,
        ARGS&&..._args)
      {
        if (get(_id)) return nullptr;
        auto&& _tool = Registry::create(_elementId,_args...);
        if (!_tool) return nullptr;
        return append(_tool,_id);
      }

      template<typename TYPE, typename...ARGS>
      Interface* append(
        const id_type& _id,
        ARGS&&..._args)
      {
        if (get(_id)) return nullptr;
        ptr_type _tool(new TYPE(_args...));
        if (!_tool) return nullptr;
        return append(_tool,_id);
      }

      bool remove(const Interface::id_type& _id)
      {
        if (!ids_.count(_id)) return false;
        auto _it = ids_[_id];
        ptrs_.erase(_it->get());
        ids_.erase(_id);
        list_type::erase(_it);
        return true;
      }

      bool changeId(
        const Interface::id_type& _oldId,
        const Interface::id_type& _newId)
      {
        if (!ids_.count(_oldId)) return false;
        auto _it = ids_[_oldId];
        if (!ptrs_.count(_it->get())) return false;
        ptrs_.erase(_it->get());
        ids_.erase(_oldId);
        ids_.insert( {_newId,_it});
        ptrs_.insert( {_it->get(),_newId});
        return true;
      }

      bool remove(Interface* _tool)
      {
        if (!ptrs_.count(_tool)) return false;
        auto _id = ptrs_[_tool];
        auto _it = ids_[_id];
        ptrs_.erase(_it->get());
        ids_.erase(_id);
        list_type::erase(_it);
        return true;
      }

    private:
      template<typename TOOL, typename ID>
      Interface* append(TOOL&& _tool, ID&& _id)
      {
        list_type::push_back(std::move(_tool));
        ptrs_.insert( {list_type::back().get(),_id});
        ids_.insert( {_id,--list_type::end()});
        return list_type::back().get();
      }

      template<typename TOOL, typename ID, typename ITERATOR>
      Interface* insert(TOOL&& _tool, ID&& _id, ITERATOR _it)
      {
        auto&& _newIt = list_type::insert(_it,std::move(_tool));
        ptrs_.insert( {list_type::back().get(),_id});
        ids_.insert( {_id,_newIt});
        return _newIt->get();
      }
    };
  }
}


