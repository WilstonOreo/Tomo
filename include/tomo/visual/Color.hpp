#pragma once

#include <gex/color.hpp>
#include <tbd/serialize.h>

namespace tomo
{
  namespace visual
  {
    typedef gex::Color3<> Color3;
    typedef gex::Color4<> Color4;
  }

  using visual::Color3;
  using visual::Color4;
}

namespace tbd
{ 
  template<>
  struct Serialize<tomo::Color3>
  {
    template<typename CONFIG>
    static bool load(tomo::Color3& _c, const tbd::ConfigPath& _path, CONFIG const& _config)
    {
      auto&& _clrStr = _config.template get_optional<std::string>(_path);
      if (!_clrStr) return false;

      std::istringstream _is(_clrStr.get());
      _is >> _c.r() >> _c.g() >> _c.b();

      return true;
    }

    template<typename CONFIG>
    static void save(const tomo::Color3& _c, const tbd::ConfigPath& _path, CONFIG& _config)
    {
      std::ostringstream _os;
      _os << _c.r() << " " << _c.g() << " ";
      _os << _c.b() << " ";
      _config.put(_path,_os.str());
    }
  };
  
  template<>
  struct Serialize<tomo::Color4>
  {
    template<typename CONFIG>
    static bool load(tomo::Color4& _c, const tbd::ConfigPath& _path, CONFIG const& _config)
    {
      auto&& _clrStr = _config.template get_optional<std::string>(_path);
      if (!_clrStr) return false;

      std::istringstream _is(_clrStr.get());
      _is >> _c.r() >> _c.g() >> _c.b() >> _c.a(); 

      return true;
    }

    template<typename CONFIG>
    static void save(const tomo::Color4& _c, const tbd::ConfigPath& _path, CONFIG& _config)
    {
      std::ostringstream _os;
      _os << _c.r() << " " << _c.g() << " ";
      _os << _c.b() << " " << _c.a() << " ";
      _config.put(_path,_os.str());
    }
  };
}
