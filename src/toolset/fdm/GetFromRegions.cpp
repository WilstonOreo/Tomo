#include <tomo/item/SourceAndSink.hpp>

#include <tomo/item/Regions.hpp>
#include <tomo/slice/for_each.hpp>
#include <tomo/tool/Tool.hpp>

#include "item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    class GetFromRegions : public Tool<GetFromRegions>
    {
      TOMO_PARAMETER_LIST 
      (
        (Source<item::SliceGraph>) source,
        (Source<tomo::item::Regions>) regions,
        (Sink<item::SliceGraph>) hit,
        (Sink<item::SliceGraph>) miss,
        (std::string) mode /* intersects (default), within */ 
      )
      typedef tomo::item::Regions::node_type node_type;
    
    public:
      TOMO_TOOL("fdm",GetFromRegions)

      GetFromRegions(parent_type const* _parent) :
        Tool<GetFromRegions>(_parent),
        hit_(_parent),
        miss_(_parent),
        mode_("intersects") {}
      
      bool valid() const
      {
        return source() && regions() && (hit() || miss());
      }

      void process()
      {
        hit_->data().clear();
        miss_->data().clear();
        fdm::SliceGraph const& _sliceGraph = source()->data();
        tomo::slice::for_each(_sliceGraph,[&](const fdm::Slice& _slice)
          {
            auto&& _regions = query(_slice.bounds());
            bool _hit = false;
            for (auto& _region : _regions)
            {
              gex::Bounds2&& _bounds = _region.first.project(gex::Z);
              _hit |= gex::intersects(_bounds,_slice.polygon());
            }
            insert(_slice,_hit);
            if (hit()) hit_->data().connect();
            if (miss()) miss_->data().connect();
          });
        }

    private:
      template<typename BOUNDS>
      std::vector<node_type> query(const BOUNDS& _bounds)
      {
        std::vector<node_type> _result;
        auto const& _regions = regions()->data();
        _regions.query(gex::index::intersects(_bounds),std::back_inserter(_result));
        return _result;
      }
      
      template<typename T>
      void insert(const T& _t, bool _hit)
      {
        if (_hit)
        {
          if (hit()) hit_->data().insert(_t);
        } else
        {
          if (miss()) miss_->data().insert(_t);
        }
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::GetFromRegions)
