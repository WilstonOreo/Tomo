#pragma once

#include <tbd/config.h>
#include <tbd/parameter.h>
#include <tbd/parse_utils.h>

namespace tomo
{
  namespace base
  {
    namespace util
    {
      template<typename ELEMENT>
      void getConfigParameters(ELEMENT& _element)
      {
        TOMO_DBG("")
        // Get default values from parent (if element has parent and parent has a config)
        if (!_element.parent()) return;
        TOMO_DBG("Has Parent")
        auto _config = _element.parent()->getConfig();
        if (!_config) return;

        TOMO_DBG("Has Config with id " << _element.parent()->configId() << ", Element has id: " << _element.getTypeId());
        _element.load(tbd::ConfigPath(_element.getTypeId()),*_config);
      }

      template<typename CONFIG_ITEM, typename ELEMENT, typename TOKENS>
      void parse(ELEMENT& _element, const TOKENS& _tokens)
      {
        getConfigParameters(_element);

        for (auto& _token : _tokens)
        {  
          auto&& _keyValue = tbd::splitToken(_token,'=');

          // Example: MyTool tool(config=global:"/path/to/parameters")
          // Example: MyTool tool2(configfile="myconfig.cfg":"/path/to/parameters")
          if (_keyValue.first == "config" || _keyValue.first == "configfile")
          {
            auto&& _configNameAndPath = tbd::splitToken(_keyValue.second,':');
            tbd::ConfigPath _configPath = tbd::removeQuotes(_configNameAndPath.second); 
            if (_configPath.empty()) _configPath = _element.getTypeId(); 

            auto&& _configName = tbd::removeQuotes(_configNameAndPath.first);

            tbd::Config* _config = _keyValue.first == "config" && _element.parent() ? 
              _element.parent()->template value<CONFIG_ITEM>(_configName) : // Get config from parent
              new tbd::Config(_configName); // Load config from filename
            if (!_config) continue;
            _element.load(_configPath,*_config);
          } 
          else
          _element.put({_token});
        }
      }
    }
  }
}

