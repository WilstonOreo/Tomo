
#include <tomo/base/debug.hpp>

#include <tomo/item/SourceAndSink.hpp>
#include <tomo/tool/Tool.hpp>
#include "tool/perimeter.hpp"

namespace tomo
{
  namespace fdm
  {
    class Perimeter : public Tool<Perimeter>
    {
      TOMO_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) sink,
        (Sink<item::SliceGraph>) inner,
        (int) number,
        (float) speed,
        (float) extrusion_width,
        (int) path_tag
      )
    public:
      TOMO_TOOL("fdm",Perimeter)

      Perimeter(parent_type const* _parent) :
        Tool<Perimeter>(_parent), 
        sink_(_parent),
        inner_(_parent),
        number_(3),
        speed_(50.0),
        extrusion_width_(0.5),
        path_tag_(0) {}

      bool valid() const
      {
        return source();
      }

      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;
        make(*this,_cfg,"number",{
            {"min","1"},
            {"max","100"}});
        make(*this,_cfg,"speed",{
            {"min","30"},
            {"max","120"}});
        make(*this,_cfg,"extrusion_width",{
            {"min","0.10"},
            {"max","0.35"}});
      }

      void process()
      {
        if (sink())
        {
          perimeter(
              source()->data(),
              strategy::perimeter::Default(number(),speed(),extrusion_width(),path_tag()),
              sink_->data());
        }
        if (inner())
        {
          perimeter(
              source()->data(),
              strategy::perimeter::Inner(number(),extrusion_width()),
              inner_->data());
        }
      }
    };
  }
}


TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::Perimeter)
