#define TOMO_PLUGIN

#include <tomo/base/debug.hpp>
#include <gex/prim.hpp>
#include <gex/algorithm/boost.hpp>
#include <gex/algorithm/offset.hpp>
#include <gex/algorithm/within.hpp>
#include <gex/algorithm/bounds.hpp>

#include <tomo/slice/for_each_parallel.hpp>
#include <tomo/slice/for_each_layer.hpp>
#include <tomo/routing/generate.hpp>
#include "routing/MoveMaker.hpp"


//#include <gex/algorithm/offset_details.hpp>
#include <gex/algorithm/perimeter.hpp>
#include <tomo/item/SourceAndSink.hpp>
#include <tomo/tool/Tool.hpp>

#include "item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    class BetterPerimeter : public Tool<BetterPerimeter>
    {
      TOMO_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) sink,
        (Sink<item::SliceGraph>) inner,
        (int) number,
        (float) speed,
        (float) outer_speed,
        (float) extrusion_width,
        (float) min_extrusion_width,
        (float) max_extrusion_width,
        (int) path_tag,
        (bool) handle_sampling_artifacts,
        (bool) adapt_extrusion_width
      )
    public:
      TOMO_TOOL("fdm",BetterPerimeter)
      
      BetterPerimeter(parent_type const* _parent) :
        Tool<BetterPerimeter>(_parent),
        sink_(_parent),
        inner_(_parent),
        number_(3),
        speed_(50.0),
        outer_speed_(30.0),
        extrusion_width_(0.5),
        min_extrusion_width_(0.1),
        max_extrusion_width_(0.8),
        path_tag_(0),
        handle_sampling_artifacts_(false),
        adapt_extrusion_width_(true) {}
      
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
        make(*this,_cfg,"outer_speed",{
            {"min","15"},
            {"max","120"}});
        make(*this,_cfg,"min_extrusion_width",{
            {"min","0.10"},
            {"max","0.35"}});
        make(*this,_cfg,"max_extrusion_width",{
            {"min","0.10"},
            {"max","0.35"}});
        make(*this,_cfg,"handle_sampling_artifacts");
        make(*this,_cfg,"adapt_extrusion_width");
      }

      void process()
      {
        TOMO_DBG("BetterPerimeter")
        
        using namespace tomo::slice;
        typedef strategy::for_each::Parallel<fdm::SliceGraph> strategy_type;

        if (sink())
        {
          sink_->clear();
          strategy_type _parallel(sink_->data());
          using namespace tomo::slice;
          for_each(source()->data(),_parallel,[&](const fdm::Slice& _slice) -> std::vector<fdm::Slice>
          {
            auto& _polygon = _slice.polygon();
            gex::MultiLineString _details;
            typedef gex::strategy::offset::MultiWithDetails<gex::Point2> strategy_type;
            typedef typename strategy_type::indices_type indices_type;

            indices_type _indices;
            auto&& _perimeters = gex::offset(_slice.polygon(),strategy_type(extrusion_width(),number(),_details,_indices));
            std::vector<fdm::Path> _detailPaths;
            Slice _perimeterSlice(_slice.range(),_slice.polygon(),path_tag());
            int _index = 0;
            
            /// We need indices to decrease speed for outer perimeters on the fly
            for (auto& _range : _indices)
            {
              float _speed = _index == 0 ? outer_speed() : speed();
              for (size_t i = _range.first; i < _range.second; ++i)
              {
                auto& _p = _perimeters[i];
                _p.update();
                gex::Polygon _simplified;
                gex::algorithm::simplify(_p,_simplified,extrusion_width()*0.2);
                _simplified.update();
                routing::generate(_simplified,fdm::MoveMaker(_slice,_speed,extrusion_width()),_perimeterSlice);
              }
              ++_index;
            }
          
            if (handle_sampling_artifacts())
            {
              for (auto& _lineString : _details)
              {
                auto&& _bounds = gex::bounds(_lineString);
                if (!gex::within(_bounds,_polygon.bounds())) continue;

                fdm::Path _path;
                routing::generate(_lineString,fdm::MoveMaker(_slice,outer_speed(),extrusion_width()),_path);
                if (adapt_extrusion_width()) 
                {
                  routing::for_each_move(_path,[&](fdm::Move& _move)
                  {
                    gex::Point2 _movePoint = gex::Point2(_move.x(),_move.y());
                    gex::Point2 _p = gex::closestPoint(_polygon,_movePoint); 
                    auto _adaptedExtrusionWidth = 2.0*gex::distance(_p,_movePoint);
                    if (_adaptedExtrusionWidth > max_extrusion_width()) return;
                    if (_adaptedExtrusionWidth < min_extrusion_width())
                    {
                      _adaptedExtrusionWidth = min_extrusion_width();
                    }
                    _move.extrusionWidth(_adaptedExtrusionWidth);
                  });
                }
                if (!_path.empty())
                  _perimeterSlice.insert(_path); 
              }
            }

            return {_perimeterSlice};
          });
          sink_->data().connect();
        }
        if (inner())
        {
          inner_->clear();
          strategy_type _parallel(inner_->data());
          for_each(source()->data(),_parallel,[&](const fdm::Slice& _slice)
          {
            std::vector<fdm::Slice> _slices;
            gex::MultiPolygon _out;
            auto&& _offsetPolygons = gex::offset(_slice.polygon(),gex::strategy::offset::Default(-extrusion_width()*(number())));
            for (auto& _p : _offsetPolygons)
            {
              _p.update();
              if (_p.boundary().empty()) continue;
              _slices.emplace_back(_slice.range(),_p);
            }
            return _slices;
          });
          inner_->data().connect();
        }
      }
    };
  }
}

      
TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::BetterPerimeter)      
