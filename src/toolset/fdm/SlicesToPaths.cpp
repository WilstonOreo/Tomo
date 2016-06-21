#include <tomo/item/SourceAndSink.hpp>
#include <tomo/tool/Tool.hpp>
#include "item/SliceGraph.hpp"
#include "routing/MoveMaker.hpp"

namespace tomo
{
  namespace fdm
  {
    class SlicesToPaths : public Tool<SlicesToPaths>
    {
      TBD_PARAMETER_LIST 
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) sink,
        (Scalar) speed,
        (Scalar) extrusion_width,
        (Scalar) simplify
      )
    public:
      TOMO_TOOL("fdm",SlicesToPaths)

      SlicesToPaths(parent_type const* _parent) :
        Tool<SlicesToPaths>(_parent),
        sink_(_parent),
        speed_(70.0),
        extrusion_width_(0.5),
        simplify_(0.0) {}
      
      bool valid() const
      {
        return source() && sink();
      }

      void process()
      {
        fdm::SliceGraph const& _in = source()->data();
        fdm::SliceGraph& _out = sink_->data();
        _out.clear();
        typedef fdm::Slice::polygon_type polygon_type;

        tomo::slice::for_each(_in,[&](const fdm::Slice& _slice)
        {
          polygon_type _simplePolygon;
          if (simplify() > 0.0)
          {
            gex::algorithm::simplify(
              _slice.polygon(),
              _simplePolygon,
              simplify()*extrusion_width()/100.0);
          }
          polygon_type const& _polygon = simplify() > 0.0 ? _simplePolygon : _slice.polygon();
          
          Slice _newSlice(_slice.range(),_slice.polygon(),_slice.tag());
          routing::generate(_polygon,
            fdm::MoveMaker(_slice,speed(),extrusion_width()),_newSlice);
          _out.insert(_newSlice);
        });
        _out.connect();
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::SlicesToPaths)
