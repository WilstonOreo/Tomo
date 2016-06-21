#pragma once

#include "gl.hpp"
#include <gex/base.hpp>
#include "Visualizer.hpp"
#include "gl/bounds.hpp"
#include <tomo/item/Regions.hpp>

namespace tomo
{
  namespace visual
  {
    class Regions : 
      public Visualizer<Regions,item::Regions>
    {
      TBD_PARAMETER_LIST
      (
        (ColorRange3) color,
        (bool) show_bounds
      )
    public:
      TOMO_VISUALIZER(item::Regions)

      Regions(Element const* _regions) : 
        Visualizer<Regions,item::Regions>(_regions),
        color_(gex::Range(0.0,1.0),Color3(0.0,0.0,0.0),Color3(1.0,1.0,1.0)),
        show_bounds_(false) {}

      inline void preprocess()
      {
        item::Regions const* _item = this->get();
        regionsBounds_ = gex::Bounds3();
        regions_.clear();

        auto _all = [](const node_type& _node) { return true; };
        _item->data().query(gex::index::satisfies(_all),std::back_inserter(regions_));
        for (auto& _region : regions_)
        {
          regionsBounds_.extend(_region.first);
        }
      }

      gex::Bounds3 bounds() const { return regionsBounds_; }

      inline void process()
      {
        if (regions_.empty()) return;
        if (show_bounds())
          gl::bounds(regionsBounds_,Color3(1,1,1)); 
        for (auto& _region : regions_)
        {
          gl::bounds(_region.first,color_.interpolate(_region.second));
        }
      }

    private:
      typedef std::pair<Bounds3,Scalar> node_type;
      std::vector<node_type> regions_;
      gex::Bounds3 regionsBounds_;

    };
  }
}
