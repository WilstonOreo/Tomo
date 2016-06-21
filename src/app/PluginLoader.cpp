#include "PluginLoader.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/filesystem.hpp>
#include <tomo/base/debug.hpp>
#include <dlfcn.h>

namespace tomo
{
  void PluginDeleter::operator()(void * p)
  {
    dlclose(p);
  }

  PluginLoader* PluginLoader::instance_ = PluginLoader::instance();

  template<typename PATH>
  std::vector<std::string> filesWithExt( const PATH& _path,         // in this directory,
                                         const std::string& _fileExt ) // search for this name,
  {
    using namespace boost::filesystem;
    auto&& _upperExt = boost::algorithm::to_upper_copy(_fileExt);
    std::vector<std::string> _files;
    if ( !exists( _path ) ) return _files;
    directory_iterator end_itr;
    for ( directory_iterator itr( _path );
          itr != end_itr;
          ++itr )
    {
      auto&& _upper = boost::algorithm::to_upper_copy(itr->path().string());
      if ( is_directory(itr->status()) )
      {
        auto&& _dirFiles = filesWithExt( itr->path(), _fileExt );
        for (auto& _dirFile : _dirFiles)
          _files.push_back(_dirFile);
      }
      else if ( boost::algorithm::ends_with(_upper,_upperExt) ) 
      {
        _files.push_back(itr->path().string());
      }
    }
    return _files;
  }

  void PluginLoader::loadInPath(const std::string& _path)
  {
    auto&& _pluginSOs = filesWithExt(boost::filesystem::path(_path),fileExt());
    for (auto& _pluginSO : _pluginSOs)
    {
      load(_pluginSO);
    }
  }

  bool PluginLoader::load(const std::string& _filename)
  {  
    if (plugins().count(_filename) > 0) return false;
    TOMO_DBG("Loading plugin: " << _filename);

    // Load the plugin's .so file
    void *_handle = dlopen(_filename.c_str(),  RTLD_NOW);

    if(!_handle)
    {
      TOMO_ERR("Plugin: " << dlerror());
      return false;
    }
    dlerror();

    // Get the pluginConstructor function
    auto reg = (void(*)(void)) dlsym(_handle, "reg");

    char *_error = NULL;
    if((_error = dlerror()))
    {
      TOMO_ERR("Failed to load: '" << _filename << "'!")
      dlclose(_handle);
      return false;
    }
    plugins()[_filename].reset(_handle);
    reg();
    return true;
  }
  
  bool PluginLoader::unload(const std::string& _pluginId)
  {
    return plugins().erase(_pluginId) > 0;
  }

  std::set<std::string> PluginLoader::loaded()
  {
    std::set<std::string> _ids;
    for (auto& _plugin : plugins())
    {
      _ids.insert(_plugin.first);
    }
    return _ids;
  }

  std::string PluginLoader::fileExt()
  {
#ifdef __APPLE__
    return ".dylib";
#else
    return ".so";
#endif

  }
}

