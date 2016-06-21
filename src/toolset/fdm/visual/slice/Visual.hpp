#pragma once

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif


#include "../../item/SliceGraph.hpp"
#include <boost/optional.hpp>

namespace tomo
{
  namespace fdm
  {
    namespace visual
    {
      namespace slice
      {
        class Visual
        {
        public:
          Visual(fdm::SliceGraph const* _sliceGraph = nullptr) :
            modified_(true),
            sliceGraph_(_sliceGraph)
          {}
          virtual ~Visual() {}

          virtual void update() = 0;
          virtual void draw() = 0;
          virtual void draw(const gex::Range& _range = gex::Range()) = 0;

          void preprocess()
          {
            if (modified_)
            {
              update();
              modified_ = false;
            }
          }

          void preprocess(fdm::SliceGraph const* _sliceGraph)
          {
            sliceGraph(_sliceGraph);
            preprocess();
          }

          TBD_PROPERTY_REF(bool,modified)
          TBD_PROPERTY(fdm::SliceGraph const*,sliceGraph)
        };
      }
    }
  }
}
