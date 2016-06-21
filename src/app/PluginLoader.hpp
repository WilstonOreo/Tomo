#pragma once

#include <string>
#include <map>
#include <set>
#include <memory> 

namespace tomo
{
  struct PluginDeleter 
  {
    void operator()(void * p);
  };

  typedef std::unique_ptr<void,PluginDeleter> PluginPointer;

  class PluginLoader
  {
    static PluginLoader* instance_;

    PluginLoader() {}
    PluginLoader(const PluginLoader&) {}

    PluginLoader& operator=(const PluginLoader&)
    {
      return *this;
    }
    ~PluginLoader()
    {
      instance_ = nullptr;
    }

    typedef std::map<std::string,PluginPointer> plugin_handle_map_type;

    static plugin_handle_map_type& plugins()
    {
      static plugin_handle_map_type plugins_;
      return plugins_;
    }

  public:
    static std::set<std::string> loaded(); 
    static bool unload(const std::string& _pluginId);
    static bool load(const std::string& _filename);
    static void loadInPath(const std::string& _path);
  	static inline PluginLoader* instance()
	  {
      static PluginLoader theInstance;
      instance_ = &theInstance;
      return instance_;
	  }

    static std::string fileExt();
  };  
}
