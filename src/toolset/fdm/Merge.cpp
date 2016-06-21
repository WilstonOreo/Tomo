#include <tomo/tool/Tool.hpp>

#include "item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    class Merge : public Tool<Merge>
    {
      TBD_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source1,
        (Source<item::SliceGraph>) source2,
        (Source<item::SliceGraph>) source3,
        (Source<item::SliceGraph>) source4,
        (Sink<item::SliceGraph>) sink
      )

    public:
      typedef Tool base_type;
      typedef Serializer<Merge> serializer_type;
      TOMO_TOOL("fdm",Merge)

      Merge(parent_type const* _parent) :
        Tool<Merge>(_parent),
        sink_(_parent) {}

      bool valid() const
      {
        return true;
      }

      void process()
      {
        TOMO_DBG("Merge...")
        auto& _sink = sink_->data();
        _sink.clear();
        std::vector<item::SliceGraph const*> _s( 
          { source1_.get(), source2_.get(), source3_.get(), source4_.get() });

        for (auto& _sliceGraph : _s)
        {
          if (!_sliceGraph) continue;
          tomo::slice::for_each(_sliceGraph->data(),[&_sink](const fdm::Slice& _slice)
          {
            _sink.insert(_slice);
          });
        }
        _sink.connect();
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::Merge)
