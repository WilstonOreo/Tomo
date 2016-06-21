#include <gex/algorithm/offset.hpp>
#include <gex/algorithm/bounds.hpp>
#include "item/SliceGraph.hpp"
#include <tomo/tool/Tool.hpp>
#include <tomo/item/SourceAndSink.hpp>
#include <tomo/item/Regions.hpp>


namespace tomo
{
  namespace fdm
  { 
    class AnalyzeSampling : public Tool<AnalyzeSampling> 
    {
      TOMO_PARAMETER_LIST
      (
        (Source<item::SliceGraph>) source,
        (Sink<item::SliceGraph>) defect,
        (Sink<item::SliceGraph>) sound,
        (Sink<tomo::item::Regions>) regions,
        (float) extrusion_width
      )
    public:
      TOMO_TOOL("fdm",AnalyzeSampling)

      AnalyzeSampling(parent_type const* _parent) : 
        Tool<AnalyzeSampling>(_parent),
        defect_(_parent),
        sound_(_parent),
        regions_(_parent),
        extrusion_width_(0.35)
      {}

      bool valid() const
      {
        return source() && (defect() || sound());
      }

      void process()
      {
        using namespace gex;
        
        typedef std::vector<std::pair<size_t,size_t>> indices_type;

        indices_type _indices;
        tomo::slice::for_each(source()->data(),[&](const fdm::Slice& _slice)
        {
          typedef strategy::offset::MultiWithDetails<gex::Point2> strategy_type;
          MultiLineString _details;
          MultiPolygon _offsetPolygons;
          offset(_slice.polygon(),strategy_type(extrusion_width(),1,_details,_indices),_offsetPolygons);

          bool _defect = _details.size() > 0;
          insert(_slice,_defect);

          if (regions_) 
          {
            for (auto& _lineString : _details)
            {
              auto&& _bounds = gex::bounds(_lineString);
              regions_->insert(_bounds.expand(Z,_slice.range()),1.0);
            }
          }
        });
        if (defect()) defect_->data().connect();
        if (sound()) sound_->data().connect();
      }

      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;
        make(*this,_cfg,"extrusion_width",{
            {"min","0.01"},
            {"max","0.50"}});
      }
    private:
      void insert(const fdm::Slice& _slice, bool _defect)
      {
        if (_defect)
        {
          if (defect()) defect_->data().insert(_slice);
        } else
        {
          if (sound()) sound_->data().insert(_slice);
        }
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::AnalyzeSampling)
