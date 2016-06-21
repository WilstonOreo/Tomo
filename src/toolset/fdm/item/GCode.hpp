#pragma once

#include <tomo/item/FileSource.hpp>
#include "../tool/Set.hpp"
#include "../routing/routing.hpp"

namespace tomo
{
  namespace fdm
  {
    namespace item
    {  
      struct GCodeLoader
      {
        template<typename PATH, typename ITEM>
        void operator()(const PATH& _path, ITEM& _item)
        {
          using namespace fdm::gcode;
          Container& _gcode = _item;
          _gcode.clear();
          Interpreter _interpreter(_item.toolset()->gcodes());
          _interpreter.readFile(_path,_gcode);
        }
      };

      class GCode : 
        public fdm::Item<fdm::gcode::Container>,
        public Serializer<GCode>,
        private tomo::item::Registrar<GCode>
      {
        TBD_PARAMETER_LIST
        (
          (FileSource<GCode,GCodeLoader>) file
        )
      public:
        typedef fdm::Item<fdm::gcode::Container> base_type;
        typedef Serializer<GCode> serializer_type;
        TOMO_ITEM(GCode)

        GCode(mutable_parent_type _parent) :
          base_type(_parent),
          serializer_type(*this),
          file_(*this) {}
      };
    }
  }
}
