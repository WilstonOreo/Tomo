#pragma once

#define TOMO_PLUGIN

#include "tomo/tool/ChainInterface.hpp"
#include "tomo/tool/Set.hpp"
#include "tomo/routing/State.hpp"
#include "tomo/routing/Path.hpp"
#include "tomo/routing/Route.hpp"
#include "tomo/routing/path/generate.hpp"
#include "tomo/routing/StateIncrementer.hpp"
#include "tomo/routing/gcode.hpp"
#include "tomo/slice.hpp"
#include "tomo/routing/Move.hpp"
#include <tbd/parameter.h>
#include "tomo/item/Item.hpp"
#include "tomo/item/Storage.hpp"
#include <gex/comp.hpp>
#include <gex/io.hpp>
#include "tomo/tool/Set.hpp"
#include "tomo/item/Item.hpp"
#include "tomo/item/parameter.hpp"

#include "tomo/item/Mesh.hpp"
//#include "item/Config.hpp"


//#include "tomo/fdm/parameter.hpp"

//#include "tomo/routing/Path.hpp"
//#include "tomo/fdm/slice.hpp"


//#include "tomo/fdm/routing.hpp"
//#include "tomo/fdm/parameter.hpp"

namespace tomo
{
  namespace tool
  {
    class Test : 
      public Interface,
      public Registrar<Test>
    {
    public:
      Test(const mutable_parent_type& _parent):
        Interface(_parent) {}

      typedef Set toolset_type;

      bool valid() const { return true; }

      void process() 
      {
        std::cout << "Hello World!" << std::endl;
      }
      
      bool hasItems() const { return false; }

      void parse(std::istream& _is)
      {
      }

      void print(std::ostream& _os) const
      {
        std::cout << "()" << std::endl;
      }

      TOMO_TOOL_TYPEID_DECL("Test")
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::tool::Test)

