#pragma once

#include "Draw.hpp"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

namespace tomo
{
  namespace gl
  {
    struct Buffer 
    {
      typedef gex::base::Model<3,GLdouble> model_type;
      typedef typename model_type::scalar_type scalar_type;
      typedef gex::base::Point<model_type> point_type;
      typedef std::vector<point_type>           point_vector;
      typedef typename point_vector::iterator   point_iterator;

      Buffer(int _numPoints) :
        tess_(0),
        points_(_numPoints)
      {
        if( _numPoints > 0 )
        {
          tess_ = gluNewTess();
          {
            gluTessCallback(tess_, GLU_TESS_VERTEX, (GLvoid (*) ()) glVertex3dv);
            gluTessCallback(tess_, GLU_TESS_BEGIN, (GLvoid (*) ()) glBegin);
            gluTessCallback(tess_, GLU_TESS_END, (GLvoid (*) ()) glEnd);
            gluTessProperty(tess_, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
          }
          // start point cache
          pointIterator_ = points_.begin();
        }
      }
      ~Buffer()
      {
        if (tess_ != 0)
          gluDeleteTess(tess_);
      }

      TBD_PROPERTY(GLUtesselator*,tess);
      TBD_PROPERTY(point_vector,points);
      TBD_PROPERTY_REF(point_iterator,pointIterator);
    };
  }
}

