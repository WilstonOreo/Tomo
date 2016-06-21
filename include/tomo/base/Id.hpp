#pragma once
#include <sstream>
#include <set>
#include <algorithm>
#include <string>      
#include <locale>      
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <tbd/property.h>

namespace tomo
{
  namespace base
  {
    /// Item Id type
    typedef std::string Token;

    struct Id
    {
      Id() {}

      template<typename STR>
      Id(const STR& _str) 
      {
        str_ = sanitize(_str);
      }

      operator bool() const
      {
        return valid();
      }

      operator std::string() const 
      {
        return str();
      }
       
      
      friend std::istream & operator >> ( std::istream& _is, Id& _s )
      {
        _is >> _s.str_;
        _s.str_ = _s.sanitize(_s.str_);
        return _is;
      }

      friend std::ostream & operator << ( std::ostream & _os, const Id& _s )
      {
        _os << _s.str();
        return _os;
      }

      bool empty() const
      {
        return !valid();
      }
      
      friend bool operator<(const Id& _lhs, const Id& _rhs)
      {
        return _lhs.str_ < _rhs.str_;
      }
      
      friend bool operator==(const Id& _lhs, const Id& _rhs)
      {
        return _lhs.str() == _rhs.str();
      }

      bool valid() const { return valid(str()); }

      template<typename STR>
      static bool valid(const STR& _str)
      {
        if (_str.empty()) return false;
        size_t _idx = 0;
        bool _valid = true;
      //  std::cout << "DEBUG: " << _str << std::endl;
        for (auto& _c : _str)
        {
          _valid &= (_c == '_') || 
            ((_idx == 0) && (std::isalpha(_c))) ||
            ((_idx != 0) && (std::isalnum(_c)));
          if (!_valid) return false;
          ++_idx;
        }
        return _valid;
      }

      static std::string sanitize(const std::string& _str)
      {
        if (_str.empty()) return std::string("");
        auto _s = _str;
        size_t _idx = 0;
        for (auto& _c : _s)
        {
          _c =  (_c == '_') || 
            ((_idx == 0) && (std::isalpha(_c))) ||
            ((_idx != 0) && (std::isalnum(_c))) ? _c : '_';
          ++_idx;
        }
        return _s;
      }

      TBD_PROPERTY_REF_RO(std::string,str)
    };

    struct ElementId
    {
      typedef Id id_type;
       
      ElementId(char const* _c_str) : 
        str_(_c_str)
      {
      }
      
      ElementId(const std::string& _str) : 
        str_(_str)
      {}

      ElementId(
          const id_type& _toolset,
          const id_type& _typeId)  
      {
        str_ = _toolset.str() + separator() + _typeId.str();
      }

      friend std::istream & operator >> ( std::istream& _is, ElementId& _s )
      {
        _is >> _s.str_;
        return _is;
      }

      friend std::ostream & operator << ( std::ostream & _os, const ElementId& _s )
      {
        _os << _s.str();
        return _os;
      }

      operator std::string const&() const 
      {
        return str();
      }
      
      operator bool() const
      {
        return valid();
      }

      bool valid() const 
      { 
        id_type _toolset, _typeId;
        parse(_toolset,_typeId);
        return _toolset.valid() && _typeId.valid(); 
      }

      std::string const& str() const
      {
        return str_;
      }

      friend bool operator<(const ElementId& _lhs, const ElementId& _rhs)
      {
        return _lhs.str() < _rhs.str();
      }
      
      friend bool operator==(const ElementId& _lhs, const ElementId& _rhs)
      {
        return _lhs.str() == _rhs.str();
      }

      static std::string separator() { return ":"; }
      
      id_type toolset() const 
      {
        id_type _toolset, _typeId;
        parse(_toolset,_typeId);
        return _toolset;
      }
      
      id_type typeId() const 
      {
        id_type _toolset, _typeId;
        parse(_toolset,_typeId);
        return _typeId;
      }
      
    private:
      std::string str_;
      void parse(id_type& _toolset, id_type& _typeId) const
      {
        std::vector<std::string> _strs;
        boost::algorithm::split(_strs,str_,boost::is_any_of(separator()));
        
        if (_strs.size() == 1)
        {
          _toolset = id_type(_strs[0]);
          _typeId = "";
          return;
        }

        if (_strs.size() == 2)
        {
          _toolset = id_type(_strs[0]);
          _typeId = id_type(_strs[1]);
        }
      }

    };
    
    typedef std::set<Id> IdSet;
    typedef std::set<ElementId> ElementIdSet;
    
    /// Generate a unique id in an idset with a certain prefix
    template<typename ID, typename IDS>
    ID genId(const ID& _prefix, const IDS& _ids)
    {
      int i = 1;
      auto _genId = _prefix.str();  
      while (_ids.count(_genId + std::to_string(i)))
      {
        ++i;
      }
      return ID(_genId + std::to_string(i));
    }

    /// Generate multiple unique ids
    template<typename ID, typename IDS>
    IDS genIds(const ID& _prefix, const IDS& _ids, size_t _number)
    {
      IDS _genIds;
      for (size_t i = 0; i < _number; ++i)
        _genIds.insert(genId(_prefix,_ids));
      return _genIds;
    }
  }

  using base::Id;
  using base::ElementId;
  using base::ElementIdSet;
}

/**@brief Macro for declaring a typeId for a class. 
 * @detail To be declared inside public section in a class
 **/
#define TOMO_TYPEID_DECL(type_id)\
    static id_type typeId() { return type_id; }\
    inline virtual id_type getTypeId() const { return typeId(); }
