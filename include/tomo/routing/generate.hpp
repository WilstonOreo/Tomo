#pragma once

#include <gex/algorithm/for_each.hpp>
#include "iterate.hpp"
#include "Path.hpp"

namespace tomo
{
  namespace routing
  {
    namespace functor
    {
      template<typename INPUT, typename PATHS>
      struct Generate {};

      template<typename POINT, typename MOVE>
      struct Generate<gex::prim::Segment<POINT>,Path<MOVE>>
      {
        typedef Path<MOVE> path_type;
        template<typename PRIMITIVE, typename MOVEMAKER_FUNCTOR>
        void operator()(
          const PRIMITIVE& _prim, MOVEMAKER_FUNCTOR p, path_type& _path)
        {
          _path.clear();
          gex::algorithm::for_each<POINT>(_prim,[&_path,&p](const POINT& _p)
          {
            MOVE _move;
            p(_p,_move);
            _path.push_back(_move);
          });
          _path.update();
        }
      };

      template<typename POINT, typename SLICE>
      struct Generate<gex::prim::Segment<POINT>,SLICE>
      {
        template<typename PRIMITIVE, typename MOVEMAKER_FUNCTOR>
        void operator()(
          const PRIMITIVE& _prim,
          MOVEMAKER_FUNCTOR p,
          SLICE& _slice)
        {
          typedef typename SLICE::path_type path_type;
          path_type _path;
          Generate<PRIMITIVE,path_type>()(_prim,p,_path);
          _slice.insert(_path);
        }
      };

      template<typename POINT, typename SLICE>
      struct Generate<gex::prim::Ring<POINT>,SLICE> :
          Generate<gex::prim::Segment<POINT>,SLICE> {};

      template<typename POINT, typename SLICE>
      struct Generate<gex::prim::LineString<POINT>,SLICE> :
          Generate<gex::prim::Segment<POINT>,SLICE> {};

      template<typename RING, typename SLICE>
      struct Generate<gex::prim::Polygon<RING>,SLICE>
      {
        template<typename PRIMITIVE, typename MOVEMAKER_FUNCTOR>
        void operator()(
          const PRIMITIVE& _prim,
          MOVEMAKER_FUNCTOR p,
          SLICE& _slice)
        {
          gex::algorithm::for_each<RING>(_prim,[&](const RING& _ring)
          {
            Generate<RING,SLICE>()(_ring,p,_slice);
          });
        }
      };

      template<typename PRIMITIVE, typename SLICE>
      struct Generate<std::vector<PRIMITIVE>,SLICE>
      {
        template<typename PRIMITIVES, typename MOVEMAKER_FUNCTOR>
        void operator()(
          const PRIMITIVES& _primitives,
          MOVEMAKER_FUNCTOR p,
          SLICE& _slice)
        {
          for (auto& _prim : _primitives)
          {
            typedef typename std::decay<decltype(_prim)>::type primitive_type;
            Generate<primitive_type,SLICE>()(_prim,p,_slice);
          }
        }
      };

      template<typename POLYGON, typename SLICE>
      struct Generate<gex::prim::MultiPolygon<POLYGON>,SLICE> :
          Generate<std::vector<POLYGON>,SLICE> {};

      template<typename POINT, typename SLICE>
      struct Generate<gex::prim::MultiSegment<POINT>,SLICE> :
          Generate<std::vector<gex::prim::Segment<POINT>>,SLICE> {};
    }

    using functor::Generate;

    template<typename PRIMITIVE, typename MOVEMAKER_FUNCTOR, typename PATHS>
    void generate(const PRIMITIVE& _p, MOVEMAKER_FUNCTOR _projFunc, PATHS& _paths)
    {
      Generate<PRIMITIVE,PATHS>()(_p,_projFunc,_paths);
    }

    template<typename PATH, typename STATE, typename GCODE, typename INCREMENTER, typename INTERPRETER>
    void generate(const Route<PATH,STATE>& _route, GCODE& _gcode, INTERPRETER _interp, INCREMENTER _inc)
    {
      iterate(_route,[&](const STATE& _lastState, const STATE& _state)
      {
        auto _insert = [&](const gcode::Command& _cmd)
        {
          if (!_cmd.empty())
            _gcode.push_back(_state,_cmd);
        };
        _interp(_lastState,_state,_insert);
      },_inc);
    }
  }
}
