#include <tomo/item/SourceAndSink.hpp>
#include <tomo/tool/Tool.hpp>
#include <tomo/slice/for_each.hpp>
#include <tomo/slice/for_each_layer.hpp>

#include "item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    class SplitLayers : public Tool<SplitLayers>
    {
      TOMO_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) even,
        (Sink<item::SliceGraph>) odd,
        (float) layer_height
      )
    public:
      TOMO_TOOL("fdm",SplitLayers)

      SplitLayers(parent_type const* _parent) :
        Tool<SplitLayers>(_parent),
        even_(_parent),
        odd_(_parent),
        layer_height_(0.0) {}

      bool valid() const
      {
        return source() && (even() || odd());// && base::util::valid(sinks());
      }

      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;
        make(*this,_cfg,"layer_height",{
            {"min","0.1"},
            {"max","10.0"}});
      }


      void process()
      {
        fdm::SliceGraph& _even = even_->data();
        if (even()) _even.clear();
        fdm::SliceGraph& _odd = odd_->data();
        if (odd()) _odd.clear();

        TOMO_DBG("Splitting layers...")
        fdm::SliceGraph const& _in = source()->data();
        auto _layerHeight = layer_height() == 0.0 ? 
          source()->layer_height() : layer_height();

        int _layerIdx = 0;

        tomo::slice::for_each_layer(_in,tomo::slice::strategy::LayerHeightBottomTop(_layerHeight),[&](const fdm::Layer& _layer)
        {
          tomo::slice::for_each(_layer,[&](const fdm::Slice& _slice)
          {
            if ((_layerIdx % 2 == 0) && even())
            {
              _even.insert(_slice); 
            }
            if ((_layerIdx % 2 == 1) && odd())
            {
              _odd.insert(_slice); 
            }

          });
          ++_layerIdx;   
        });
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::SplitLayers)
