#include <map>
#include <vector>
#include <set>

#include <tomo/base/debug.hpp>
#include <tbd/classregister.h>
#include <tbd/simple_serialization.h>

#include <tbd/boolean.h>

/// Tool Library

#include <tomo/tool/Chain.hpp>
#include <tomo/tool/Tool.hpp>
#include <tomo/item/Item.hpp>
#include <tomo/device/Device.hpp>

namespace tomo
{
  namespace strings
  {
    class String : public Item<String>
    {
      TOMO_PARAMETER_LIST
      (
        (std::string) string
      )
    public:
      TOMO_ITEM("string",String)

      String(parent_type const* _parent) :
        Item<String>(_parent) {}

      void setString(const std::string& _str)
      {
        string_ = _str;
      }
      
      bool loadFromFile(std::string const& _filename)
      {
        string_ = _filename;
        return true;
      }
    };

    class Generate :
      public Tool<Generate>
    {
      TOMO_PARAMETER_LIST
      (
        (std::string) string,
        (Sink<String>) sink
      )
    public:
      TOMO_TOOL("string",Generate)

      Generate(parent_type const* _parent) :
        Tool<Generate>(_parent),
        string_("Test."),
        sink_(_parent) {}

      void process()
      {
        sink_->setString(string_);
      }

    };

    class Concatenate :
      public Tool<Concatenate>
    {
      TOMO_PARAMETER_LIST
      (
        (Source<String>) a,
        (Source<String>) b,
        (Sink<String>) c,
        (Sink<String>) d
      )
    public:
      TOMO_TOOL("string",Concatenate)

      Concatenate(parent_type const* _parent) :
        Tool<Concatenate>(_parent),
        c_(_parent),
        d_(_parent) {}

      bool valid() const
      {
        return a() && b();
      }

      void process()
      { 
        if (c())
        {
          std::cout << "C is used" << std::endl;
          d_->setString(a()->string() + b()->string());
          std::cout << c_->string() << std::endl;
        } 
        if (d())
        {
          std::cout << "D is used" << std::endl; 
          d_->setString(a()->string() + b()->string());
          std::cout << d_->string() << std::endl;
        } 
      }
    };

    class OutputDevice :
      public Device<OutputDevice>
    {
      TOMO_PARAMETER_LIST
      (
        (Source<String>) source
      //  (FileSink) sink
      )
    public:
      TOMO_DEVICE("string",OutputDevice)

      OutputDevice(parent_type const* _parent) :
        Device<OutputDevice>(_parent) {}

      bool valid() const
      {
        return source_;
      }

      void process()
      {
        std::cout << source_->string() << " is processed..." << std::endl;
      }
    };
  }
}

template<typename REGISTRY>
void printRegistered()
{
for (auto& _class : REGISTRY::classes())
  {
    std::cout << "\t" << _class.first << std::endl;
  }
}

template<typename T>
void printTool(T const* _t)
{
  std::cout << "Sources:" << std::endl;
  auto _sources = _t->sources();
for (auto& _source : _sources)
  {
    std::cout << "\t" << _source.first << ":";
    auto&& _types = _source.second->supportedTypes();
for (auto& _type : _types)
    {
      std::cout << " " << _type;
    }
    std::cout << std::endl;
  }

  std::cout << "Sinks:" << std::endl;
  auto _sinks = _t->sinks();

  for (auto& _sink : _sinks)
  {
    std::cout << "\t" << _sink.first << ":";
    auto&& _types = _sink.second->supportedTypes();

    for (auto& _type : _types)
    {
      std::cout << " " << _type;
    }
    std::cout << std::endl;
  }
}

int main(int ac, char* av[])
{
  using namespace tomo;
/*
  std::cout << "Registered Tools:" << std::endl;
  printRegistered<tool::Registry>();
  std::cout << "Registered Items:" << std::endl;
  printRegistered<item::Registry>();
*/
  Library _library;
  _library.print();

  Toolchain _toolchain;

  std::vector<tool::Interface*> _tools;

  auto&& _item1 = _toolchain.addItem("string:String","test1");
  auto&& _item2 = _toolchain.addItem("string:String","test2");

  _item1->loadFromFile("test");
  _item2->loadFromFile("test2");
  
  _tools.push_back(_toolchain.appendTool("string:Generate"));
  _tools.push_back(_toolchain.appendTool("string:Generate"));
  _tools.push_back(_toolchain.appendTool("string:Concatenate"));

  tool::Interface* _tool = _toolchain.appendTool("string:Concatenate","test");
  if (_tool)
  {
    printTool(_tool);
  }

//  Toolchain* _subtoolchain = _toolchain.appendToolchain("Test");

  auto&& _device = _toolchain.addDevice("string:OutputDevice","my_device");


//  _toolchain.removeTool(_subtoolchain);

 TOMO_DBG(_toolchain.connect("test1",_tools[2],"a"));
 TOMO_DBG(_toolchain.connect("test2",_tools[2],"b"));
 TOMO_DBG(_toolchain.connect(_tools[2],"d",_device,"source"));

 TOMO_DBG(_toolchain.numberConnections())

//  _toolchain.connect(_tools[0],"sink",_tools[2],"a");
//  _toolchain.connect(_tools[1],"sink",_tools[2],"b");

  _toolchain.process();
  

  if (_device->valid())
  {
    _device->process();
  }

  Config _config;
  _toolchain.save("",_config);
  _config.save("test.cfg");

  _config.load("test.cfg");
  _toolchain.load("",_config);
  

  _toolchain.process();

 TOMO_DBG(_toolchain.numberConnections())


  return EXIT_SUCCESS;
}
