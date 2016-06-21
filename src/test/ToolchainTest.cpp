#include <boost/program_options.hpp>
#include <tomo/tool/Chain.hpp>
#include <tomo/tool/Library.hpp>

#include "PluginLoader.hpp"

#include <tomo/base/assert.hpp>
#include <tbd/parse_utils.h>
//#include <tomo/query/Toolchain.hpp>

using namespace boost;
namespace po = program_options;

using namespace std;
using namespace tomo;

static Library _library;

template<typename REGISTRY>
void printRegistered()
{
  for (auto& _class : REGISTRY::classes())
  {
    std::cout << "\t" << _class.first << std::endl;
  }
}

int main(int ac, char* av[])
{
  cout << "ToolchainTest -- written by Wilston Oreo." << endl;

  string _toolchainFile, _configFile, _queryFile;

  stringstream descStr;
  descStr << "Allowed options";

  // Declare the supported options.
  po::options_description desc(descStr.str());
 
  desc.add_options()
  ("help,h", "Display help message.")
  ("toolchain,t", po::value<string>(&_toolchainFile), "Toolchain file")
  ("list,l", "Shows config and lists registered tools, items")
  ("query,q", po::value<string>(&_queryFile), "Writes information about the processed toolchain into a JSON file.")
  ("parameter,p", po::value<vector<string>>()->multitoken(), "Passes a parameter to library")
  ("tool-search-path,d", po::value<vector<string>>()->multitoken(), "Custom search paths for tool plugins")
  ;
  // Parse the command line arguments for all supported options
  po::variables_map vm;
  po::store(po::parse_command_line(ac, av, desc), vm);
  po::notify(vm);
  
  // Load plugins
  if (vm.count("tool-search-path"))
  {
    auto&& _paths = vm["tool-search-path"].as<vector<string>>();
    for (auto& _path : _paths) 
      PluginLoader::loadInPath(tbd::removeQuotes(_path));
  } else
  {
    PluginLoader::loadInPath("./bin");
  }

  // Load config
  if (vm.count("config"))
  {
    _library.config().load(_configFile);
  }

  // Set config parameter
  if (vm.count("parameter"))
  {
    auto&& _parameters = vm["parameter"].as<vector<string> >();
    for (auto& _parameter : _parameters)
    {
      auto&& _tokens = tbd::splitToken(tbd::removeQuotes(_parameter),'=');
      _library.config().put(_tokens.first,_tokens.second);
    }
  }

  // List tools, items and parameters
  if (vm.count("list"))
  {
    Library::print();
  //  printRegistered<tomo::visual::Registry>();
  //  printRegistered<tomo::query::Registry>();
  }

  // Display help
  if (vm.count("help"))
  {
    cout << desc << endl;
    return EXIT_FAILURE;
  }

  /// Silent mode: Just process toolchain
  Toolchain _toolchain(&_library);
  _toolchain.loadFromFile(_toolchainFile);

  _toolchain.process();
  for (auto& _device : _toolchain.devices())
  {
    _device.second->process();
  }

  auto _device = _toolchain.addDevice("fdm:UltiMaker");
  TOMO_ASSERT(_device);
  TOMO_ASSERT(_toolchain.removeDevice(_device));
  
  //TOMO_ASSERT(_toolchain.removeTool("Sampler1"));
  
  for (int i = 0; i < 10; i++)
  {
  auto _tool = _toolchain.appendTool("fdm:Sampler");
  TOMO_ASSERT(_tool);
  TOMO_DBG("Tool Id" << _toolchain.getToolId(_tool))
  }
  //TOMO_ASSERT(_toolchain.removeTool(_tool));

  return EXIT_SUCCESS;
}
