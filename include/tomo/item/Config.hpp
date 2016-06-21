#pragma once

#include "Item.hpp"
#include "FileSource.hpp"

namespace tomo
{
  namespace item
  {
    struct ConfigLoader
    {
      template<typename PATH, typename ITEM>
      void operator()(const PATH& _path, ITEM& _item)
      { 
        _item.data().load(_path);  
      };
    };

    /// Tomo config item
    class Config : 
      public tomo::Item<tbd::Config>,
      public Serializer<Config>,
      public Registrar<Config>
    {
      TBD_PARAMETER_LIST
      (
        (FileSource<Config,ConfigLoader>) file
      )
    public:
      typedef tomo::Item<tbd::Config> base_type;
      typedef Serializer<Config> serializer_type;
      TOMO_ITEM(Config)
  
      Config(mutable_parent_type _parent) : 
        base_type(_parent),
        serializer_type(*this),
        file_(*this) {}

      Config(mutable_parent_type _parent, const data_type& _data) :
        base_type(_parent,_data),
        serializer_type(*this),
        file_(*this) {}

      inline void parse(std::istream& _is) 
      {
        // Parse tokens
        tokenlist_type _tokens;
        //tbd::parse(_is,Element::tokens_,"([",")]",",",0);
        tbd::parse(_is,_tokens,"{[(",")]}",":;",0);
        if (_tokens.empty()) return;
        std::istringstream _iss(_tokens[0]);
        
        inherited_type::parse(_iss);
        //base::util::parse<item::Config>(*this,Element::tokens_);
        /// Read config from filename

        if (_tokens.size() >= 2)
        {
          TOMO_DBG(_tokens[1])
          tbd::Config(*this).fromStr(_tokens[1]);
        }
      }
    };

    typedef Config PropertyTree;
  }
}

