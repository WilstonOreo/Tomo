#include <gex/algorithm/convert.hpp>
#include <tomo/item/Source.hpp>
#include <tomo/item/FileSink.hpp>
#include <tomo/tool/Tool.hpp>
#include "item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    class debug_WKT : 
      public Tool<debug_WKT>
    {
      TOMO_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (bool) paths,
        (bool) slices,
        (FileSink) output_file
      )
    public:
      TOMO_TOOL("fdm",debug_WKT)

      debug_WKT(parent_type const* _parent) : 
        Tool<debug_WKT>(_parent),
        output_file_("slicegraph.wkt") {} 

      bool valid() const
      {
        return source();
      }

      void process()
      {
        std::ofstream _f(output_file_.path()); 
        
        if (slices())
        {
          fdm::SliceGraph const& _sliceGraph = source()->data();
          TOMO_DBG("Write slice file to " << output_file() << "...")
          tomo::slice::for_each(_sliceGraph,[&](const fdm::Slice& _slice)
          {
            _f << boost::geometry::wkt(_slice.polygon()) << std::endl;
          });
        }
        if (paths())
        {
          fdm::SliceGraph const& _sliceGraph = source()->data();
          tomo::slice::for_each_layer(_sliceGraph,tomo::slice::strategy::LayerHeightBottomTop(_sliceGraph.minSliceHeight()),[&](const Layer& _layer)
          {
            gex::MultiLineString _lines;
            tomo::routing::for_each_path(_layer,[&_lines](const fdm::Path& _path)
            {
              gex::LineString _line;
              gex::for_each<Move>(_path,[&_line](const Move& _move) 
              {
                _line.emplace_back(_move.x(),_move.y());
              });
              _lines.push_back(_line);
            });
            _f << boost::geometry::wkt(_lines) << std::endl;
          });
        }
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::debug_WKT)

