#pragma once

#include "Visual.hpp"

namespace tomo
{
  namespace fdm
  {
    namespace visual
    {
      namespace slice
      {
        class Defect : public Visual
        {
        public:
          Defect(fdm::SliceGraph const* _sliceGraph = nullptr) :
            Visual(_sliceGraph),
            color_(Color3(1.0,0.0,0.0)) 
          {}

          void update()
          {
          }

          void draw()
          {
            using namespace tomo::visual;
            using namespace tbd::gl; 
            
            gl::color(color_);
            for (auto& _defect : sliceGraph()->defectSlices())
            {
              auto _zPos = _defect.first;
              auto& _lineStrings = _defect.second;
              gl::translate(0.0f,0.0f,_zPos);
              for (auto& _lineString : _lineStrings)
              {
                line(_lineString);
              }
              gl::translate(0.0f,0.0f,-_zPos);
            }
          }

          void draw(const gex::Range& _range) 
          {
            using namespace tomo::visual;
            using namespace tbd; 

            gl::color(color_);
            for (auto& _defect : sliceGraph()->defectSlices())
            {
              auto _zPos = _defect.first;
              if (!gex::intersects(_zPos,_range)) continue;
              auto& _lineStrings = _defect.second;
              gl::translate(0.0f,0.0f,_zPos);
              for (auto& _lineString : _lineStrings)
              {
                line(_lineString);
              }
              gl::translate(0.0f,0.0f,-_zPos);
            }
          }
          TBD_PROPERTY_REF(Color3,color)
        private:
          template<typename LINESTRING>
          static void line(const LINESTRING& _lineString)
          {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(2,GL_FLOAT,0,_lineString.data());
            glDrawArrays(GL_LINE_STRIP, 0, _lineString.size());
            glDisableClientState(GL_VERTEX_ARRAY);
          }
        };
      }
    }
  }
}
