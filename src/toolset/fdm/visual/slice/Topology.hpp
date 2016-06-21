#pragma once

#include "Visual.hpp"
#include <tomo/slice/topology.hpp>

namespace tomo
{
  namespace fdm
  {
    namespace visual
    {
      namespace slice
      {
        class Topology : public Visual
        {
        public:
          Topology(fdm::SliceGraph const* _sliceGraph = nullptr) :
            Visual(_sliceGraph),
            lineWidth_(2.0),
            slicesColor_(Color3(0.0,1.0,1.0)),
            holesColor_(Color3(1.0,0.0,1.0)),
            drawSlices_(true),
            drawHoles_(true) {}

          void update()
          {
            using namespace tomo::slice;
            holes_.clear();
            slices_.clear();
            topology(*sliceGraph(),strategy::topology::Holes(),holes_);
            topology(*sliceGraph(),strategy::topology::Slices(),slices_);
          }

          void draw() 
          {
            using namespace tomo::visual;
            typedef gex::Point3 point_type;

            using namespace tbd::gl;

            glDisable(GL_LIGHTING);
            glLineWidth( lineWidth_ );
            if (drawSlices_)
            {
              color(slicesColor_);
              gl::draw(GL_LINES,[&]()
              {
                for (auto& _p : slices_)
                {
                  vertex(_p.back());
                  vertex(_p.front());
                }
              });  
            }
            if (drawHoles_)
            {
              color(holesColor_);
              gl::draw(GL_LINES,[&]()
              {
                for (auto& _p : holes_)
                {
                  vertex(_p.back());
                  vertex(_p.front());
                }
              });
            }
          
          }
          
          void draw(const gex::Range& _range) 
          {
            draw();
          }

          TBD_PROPERTY_REF(float,lineWidth)
          TBD_PROPERTY_REF(Color3,slicesColor)       
          TBD_PROPERTY_REF(Color3,holesColor)       
          TBD_PROPERTY(bool,drawSlices)
          TBD_PROPERTY(bool,drawHoles)

        private:
          typedef typename tomo::slice::strategy::topology::Holes holes_strategy_type;
          typedef typename tomo::slice::strategy::topology::Slices slices_strategy_type;
          typedef typename tomo::slice::Topology<holes_strategy_type>::output_type holes_type;
          typedef typename tomo::slice::Topology<slices_strategy_type>::output_type slices_type;

          holes_type holes_;
          slices_type slices_;
        };
      }
    }
  }
}
