#pragma once

#include <fstream>

namespace tomo
{
  namespace item
  {
    struct FileSink
    {
      typedef std::ofstream stream_type;
     
      FileSink() {}

      template<typename PATH> 
      FileSink(const PATH& _path) :
        path_(_path) {}
      
      friend std::istream & operator >> ( std::istream& _is, FileSink& _s )
      {
        _is >> _s.path_;
        return _is;
      }

      friend std::ostream & operator << ( std::ostream & _os, const FileSink& _s )
      {
        _os << _s.path_;
        return _os;
      }

      operator bool() const
      {
        return !std::string(path_).empty();
      }

      static Id typeId() { return "FileSink"; }

      TBD_PROPERTY_REF(std::string,path)
    };
  }

  using item::FileSink;
}

namespace tbd
{
  template<>
  struct TypeInfo<tomo::item::FileSink>
  {
    typedef std::string token_type;
    inline token_type operator()()
    {
      return tomo::item::FileSink::typeId();
    }
  };
}


