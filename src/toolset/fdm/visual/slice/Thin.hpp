#pragma once

#include "Visual.hpp"
#include <gex/algorithm/offset.hpp>

namespace tomo
{
  namespace fdm
  {
    namespace visual
    {
      namespace slice
      {
        class Thin : public Visual
        {
        public:
          Thin(fdm::SliceGraph const* _sliceGraph = nullptr) :
            Visual(_sliceGraph) {}

          void update()
          {
            /*
            thinSlices_.clear();
            tomo::slice::for_each(*sliceGraph(),[&](const fdm::Slice& _slice)
            {
              auto&& _offsetPolygons = gex::offset(_slice.polygon(),-device()->nozzle_diameter());
              _offsetPolygons.update();
              if (_offsetPolygons.area() * 2.0 < _slice.polygon().area())
                thinSlices_.insert(&_slice);
            });
            TOMO_DBG(thinSlices_.size() << " thin slices.")*/
          }

          void draw(const gex::Range& _range) 
          {/*
            gl::Color(thin_slice_color());
for (auto& _slice : thinSlices_)
            {
              auto _z = _slice->range();
              auto& _p = _slice->polygon();
              glTranslatef(0,0,_z.middle());
              outline(_p);
              glTranslatef(0,0,-_z.middle());
            }*/
          }

          void draw() {}

        private:
          std::set<fdm::Slice const*> thinSlices_;
        };
      }
    }
  }
}
