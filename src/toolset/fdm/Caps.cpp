#include <gex/algorithm/intersection.hpp>
#include <tomo/slice/overlapping.hpp>
#include <tomo/item/SourceAndSink.hpp>
#include <tomo/tool/Tool.hpp>

#include "item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    class Caps : public Tool<Caps>
    {
      enum Mode { TOP_BOTTOM, TOP, BOTTOM, NONE };
      TBD_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) caps,
        (Sink<item::SliceGraph>) residues,
        (int) mode, 
        (float) height
      )

    public:
      TOMO_TOOL("fdm",Caps)

      Caps(parent_type const* _parent) :
        Tool<Caps>(_parent),
        caps_(_parent),
        residues_(_parent),
        mode_(TOP_BOTTOM),
        height_(1.0) {}

      bool valid() const
      {
        return source();
      }

      void process()
      {
        TOMO_DBG("SliceGraph Caps...")

        fdm::SliceGraph const& _sliceGraphIn = source()->data();
        fdm::SliceGraph& _caps = caps_->data();
        _caps.clear();
        
        using namespace tomo::slice;

        if (residues())
        {
          fdm::SliceGraph& _residues = residues_->data();
          _residues.clear();
          gex::MultiPolygon _capPolygons, _invCapPolygons;         
          for_each(_sliceGraphIn,[&](const fdm::Slice& _slice)
          {
            _invCapPolygons.clear();
            
            switch (mode())
            {
            case TOP: 
              _capPolygons = overlapping(_sliceGraphIn,_slice,strategy::Top_Height_Inv(height(),_invCapPolygons));
              break;
            case BOTTOM:
              _capPolygons = overlapping(_sliceGraphIn,_slice,strategy::Bottom_Height_Inv(height(),_invCapPolygons));
              break;
            case TOP_BOTTOM:
              _capPolygons = overlapping(_sliceGraphIn,_slice,strategy::TopBottom_Height_Inv(height(),_invCapPolygons));
            default: break;
            }

            for (auto& _polygon : _capPolygons)
              _caps.insert(fdm::Slice(_slice.range(),_polygon));
            for (auto& _polygon : _invCapPolygons)
              _residues.insert(fdm::Slice(_slice.range(),_polygon));
          });
          _residues.connect();
        } else
        {
          gex::MultiPolygon _capPolygons;
          for_each(_sliceGraphIn,[&](const fdm::Slice& _slice)
          {
            switch (mode())
            {
            case TOP: 
              _capPolygons = overlapping(_sliceGraphIn,_slice,strategy::Top_Height(height()));
              break;
            case BOTTOM:
              _capPolygons = overlapping(_sliceGraphIn,_slice,strategy::Bottom_Height(height()));
              break;
            case TOP_BOTTOM:
              _capPolygons = overlapping(_sliceGraphIn,_slice,strategy::TopBottom_Height(height()));
            default: break;
            }
            for (auto& _polygon : _capPolygons)
              _caps.insert(fdm::Slice(_slice.range(),_polygon));
          });
        }
        _caps.connect();
      }

      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;
        make(_cfg,"mode",{
            {"type","list"},
            {"items","top_bottom,top,bottom"}});
        make(*this,_cfg,"height",{
            {"min","0.01"},
            {"max","0.50"}});
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::Caps)
