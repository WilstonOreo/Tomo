#include <boost/program_options.hpp>
#include <tomo/tool/Chain.hpp>
#include <tomo/tool/Library.hpp>

#ifndef TOMO_NO_GUI
#include <tomo/visual/gl.hpp>
#include "MainWindow.h"
#include <QApplication>
#include <QFile>
#include <tomo/visual/Mesh.hpp>
#include <tomo/query/Interface.hpp>
#include <tomo/visual/Regions.hpp>
//#include <tomo/visual/PointCloud.hpp>

#endif

#include "PluginLoader.hpp"

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
  cout << "Tomo Slicer 0.3 -- written by Wilston Oreo." << endl;

  string _toolchainFile, _configFile, _queryFile;

  stringstream descStr;
  descStr << "Allowed options";

  // Declare the supported options.
  po::options_description desc(descStr.str());

  desc.add_options()
  ("help,h", "Display help message.")
  ("config,c",po::value<string>(&_configFile), "Load config file.")
  ("toolchain,t", po::value<string>(&_toolchainFile), "Toolchain file")
#ifndef TOMO_NO_GUI
  ("silent,s", "Silent mode - do no not show GUI")
#endif
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

  std::cout << _configFile << std::endl;

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
    printRegistered<tomo::visual::Registry>();
    printRegistered<tomo::query::Registry>();
  }

  // Display help
  if (vm.count("help"))
  {
    cout << desc << endl;
    return EXIT_FAILURE;
  }

#ifndef TOMO_NO_GUI
  if (!vm.count("silent"))
  {
    /// Non-silent mode: Start gui
    QApplication _a(ac, av);

    QFile file(":/qss_icons/rc/style.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    _a.setStyleSheet(styleSheet);

    MainWindow _w(&_library,_toolchainFile);
    _w.setWindowState(Qt::WindowMaximized);
    _w.show();
#ifdef __APPLE__
    _w.raise();
#endif
    return _a.exec();
  } else
#endif
  {
    /// Silent mode: Just process toolchain
    Toolchain _toolchain(&_library);
    if (!_toolchain.loadFromFile(_toolchainFile)) return EXIT_FAILURE;

    _toolchain.process();

    for (auto& _device : _toolchain.devices())
    {
      if (!_device.second->valid()) continue;
      _device.second->process();
    }

/*
    if (vm.count("query"))
    {
      query::Toolchain _query(_toolchain,_queryFile);
      _query.process();
    }*/
  }

  return EXIT_SUCCESS;
}
