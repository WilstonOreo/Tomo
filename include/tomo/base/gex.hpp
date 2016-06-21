#pragma once

#include <iostream>
#include <tbd/serialize.h>
#include <gex/base.hpp>

namespace tomo
{
  namespace base
  {
    using gex::Scalar;
    using gex::Axis;
    using gex::Range;
    using gex::Vec2; 
    using gex::Vec3; 
    using gex::Point2; 
    using gex::Point3; 
    using gex::Bounds2; 
    using gex::Bounds3; 
  }
  using base::Axis;
  using base::Scalar;
  using base::Range;
  using base::Vec2;
  using base::Vec3;
  using base::Point2;
  using base::Point3;
  using base::Bounds2; 
  using base::Bounds3;   
}

namespace tbd
{ 
  template<>
  struct Serialize<tomo::Vec3>
  {
    template<typename T, typename CONFIG>
    static bool load(T& _t, tbd::ConfigPath const& _path, CONFIG const& _config)
    {
      _t.x() = _config.get(_path / "x",_t.x());
      _t.y() = _config.get(_path / "y",_t.y());
      _t.z() = _config.get(_path / "z",_t.z());
      return true;
    }

    template<typename T, typename CONFIG>
    static void save(const T& _t, tbd::ConfigPath const& _path, CONFIG& _config)
    {
      _config.put(_path / "x",_t.x());
      _config.put(_path / "y",_t.y());
      _config.put(_path / "z",_t.z());
    }
  };
  
  template<>
  struct Serialize<tomo::Vec2>
  {
    template<typename T, typename CONFIG>
    static bool load(T& _t, tbd::ConfigPath const& _path, CONFIG const& _config)
    {
      _t.x() = _config.get(_path / "x",_t.x());
      _t.y() = _config.get(_path / "y",_t.y());
      return true;
    }

    template<typename T, typename CONFIG>
    static void save(const T& _t, tbd::ConfigPath const& _path, CONFIG& _config)
    {
      _config.put(_path / "x",_t.x());
      _config.put(_path / "y",_t.y());
    }
  };

  template<>
  struct Serialize<tomo::Range>
  {
    template<typename T, typename CONFIG>
    static bool load(T& _t, const tbd::ConfigPath& _path, CONFIG const& _config)
    {
      auto&& _str = _config.template get_optional<std::string>(_path);
      if (!_str) return false;

      Serialize<typename std::decay<decltype(_t.min())>::type>::load(_t.min(),_path / "min",_config);
      Serialize<typename std::decay<decltype(_t.max())>::type>::load(_t.max(),_path / "max",_config);
      return true;
    }

    template<typename T, typename CONFIG>
    static void save(const T& _t, const tbd::ConfigPath& _path, CONFIG& _config)
    {
      Serialize<typename std::decay<decltype(_t.min())>::type>::save(_t.min(),_path / "min",_config);
      Serialize<typename std::decay<decltype(_t.max())>::type>::save(_t.max(),_path / "max",_config);
    }
  };
  template<>
  struct Serialize<tomo::Bounds2> : Serialize<tomo::Range> {};
  
  template<>
  struct Serialize<tomo::Bounds3> : Serialize<tomo::Range> {};  
}

