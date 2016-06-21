#include <tomo/item/SourceAndSink.hpp>
#include <tomo/tool/Tool.hpp>

#include "item/SliceGraph.hpp"
#include "tool/perimeter.hpp"

#include <gex/algorithm/intersection.hpp>
#include <tomo/slice/overlapping.hpp>
#include "tool/Pattern.hpp"

namespace tomo
{
  namespace fdm
  {
    class Shell : public Tool<Shell>
    {
    private:
      TOMO_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) inner,
        (Sink<item::SliceGraph>) shell,
        (float) cap_thickness,
        (float) perimeter_thickness,
        (float) extrusion_width,
        (float) infill_angle,
        (float) speed,
        (int) path_tag
      )
    public:
      TOMO_TOOL("fdm",Shell)

      Shell(parent_type const* _parent) :
        Tool<Shell>(_parent),
        inner_(_parent),
        shell_(_parent),
        cap_thickness_(1.0),
        perimeter_thickness_(1.0),
        extrusion_width_(0.35),
        infill_angle_(45.0),
        speed_(50.0) {}

      bool valid() const
      {
        return source();
      }

      void process()
      {
        int _numberPerimeters = std::max(int(1),int(0.5+perimeter_thickness() / extrusion_width()));

        typedef tomo::slice::strategy::for_each::Parallel2Graphs<fdm::SliceGraph> strategy_type;


        /// Generate Perimeters
        fdm::SliceGraph&& _perimeters = 
          perimeter(
              source()->data(),
              strategy::perimeter::Default(_numberPerimeters,speed(),extrusion_width(),path_tag()));

        /// Generate Top and Bottom caps
        fdm::SliceGraph _residues, _caps;
        strategy_type _parallel(_caps,_residues);
        
        tomo::slice::for_each(source()->data(),_parallel,
            [this](const fdm::Slice& _slice,
              std::vector<fdm::Slice>& _caps,
              std::vector<fdm::Slice>& _residues)
        {
          gex::MultiPolygon _capPolygons, _invCapPolygons;         
          _invCapPolygons.clear();   
          _capPolygons = overlapping(source()->data(),_slice,
            tomo::slice::strategy::TopBottom_Height_Inv(cap_thickness(),_invCapPolygons));
          for (auto& _polygon : _capPolygons)
            _caps.emplace_back(_slice.range(),_polygon);
          for (auto& _polygon : _invCapPolygons)
            _residues.emplace_back(_slice.range(),_polygon);
        });
        _residues.connect();
        _caps.connect();

        if (shell())
        {
          fdm::SliceGraph&& _inner =
            perimeter(_caps,strategy::perimeter::Inner(_numberPerimeters,extrusion_width()));
          fdm::SliceGraph& _shell = shell_->data();
          shell_->clear();
          float _layerHeight = (source()->layer_height() <= 0.0) ? 
            cap_thickness() / 4.0 : 
            source()->layer_height();

          pattern::Linear _even(infill_angle(),extrusion_width(),true);
          pattern::Linear _odd(infill_angle() + 90.0,extrusion_width(),true);

          size_t _layerId = 0;
          auto&& _bounds = _caps.bounds().project(Slice::axis());
          tomo::slice::for_each_layer(_inner,tomo::slice::strategy::LayerHeightBottomTop(_layerHeight),
            [&](const fdm::Layer& _layer)
          {
            ++_layerId;
            pattern::Linear& _pattern = _layerId % 2 ? _even : _odd;
            tomo::slice::for_each(_layer,[&](const fdm::Slice& _slice)
            {
              auto&& _lineStrings = _pattern.generate(_bounds,_slice.polygon());
              Slice _pathSlice(_slice.range(),_slice.polygon(),path_tag());
              routing::generate(_lineStrings,fdm::MoveMaker(_slice,speed(),extrusion_width()),_pathSlice);
              _shell.insert(_pathSlice);
            });
          });

          tomo::slice::for_each(_perimeters,[&](const fdm::Slice& _slice)
          {
            _shell.insert(_slice);
          });
          _shell.connect();
        }

        if (inner())
        {
          inner_->data() =
            perimeter(_residues,strategy::perimeter::Inner(_numberPerimeters,extrusion_width()));
        }
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::Shell)
