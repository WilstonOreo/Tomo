#pragma once

#include "tomo/routing/Path.hpp"
#include "Draw.hpp"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
//#include <GL/gle.h>
#endif


namespace tomo
{
  namespace gl
  {
#define TOMO_GL_STRIP(i,p,z) \
          _strip[i][0] = (p).x(); \
          _strip[i][1] = (p).y(); \
          _strip[i][2] = z;

    template<typename STATE>
    struct Draw<routing::Path<STATE>>
    {
      typedef routing::Path<STATE> path_type;

      template<typename COLOR, typename RANGE>
      void operator()(const path_type& _a, const COLOR& _color, const RANGE& _z, Buffer& _buffer)
      {/*
        typedef typename fdm::Path::state_type state_type;
        typedef typename Buffer::point_type point_type;

#ifndef __APPLE__
        state_type _initial;
        _initial.pos(_a.movements().front());
        auto _strip = new gleDouble[_a.movements().size()+2][3];
        auto it0 = _a.begin(_initial);
        auto it1 = it0;
        ++it1;
        auto it = it1;
        auto itn = it;
        auto itn_1 = it;
        int i=1;
        for (; i<_a.movements().size()+1; ++it, ++i)
        {
          TOMO_GL_STRIP(i,it->pos(),_z.middle());
          itn_1 = itn;
          itn = it;
        }
        TOMO_GL_STRIP(0,it0->pos()-(it1->pos()-it0->pos()),_z.middle());
        TOMO_GL_STRIP(i,itn_1->pos()-(itn->pos()-itn_1->pos()),_z.middle());
        i++;

        gleSetNumSides(10);
        gleSetJoinStyle(TUBE_JN_ANGLE|TUBE_NORM_MASK|TUBE_JN_CAP);
        glePolyCylinder(i, _strip, NULL, _z.length()/2);
        delete [] _strip;
#endif*/
      }
    };
  }
}


