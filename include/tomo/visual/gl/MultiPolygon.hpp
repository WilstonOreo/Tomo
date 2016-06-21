#pragma once

#include "Draw.hpp"
#include "tomo/geometry/prim/MultiPolygon.hpp"
#include "tomo/visual/gl/Polygon.hpp"
#include <GL/gl.h>
#include <GL/glut.h>

namespace tomo
{
  namespace visual
  {
    namespace gl
    {
      template<typename SCALAR>
      struct Draw<tomo::geometry::prim::MultiPolygon2<SCALAR>>
      {
        typedef SCALAR                                            scalar_type;
        typedef tomo::geometry::prim::MultiPolygon2<scalar_type>  input_polygon;
        typedef tomo::geometry::base::Point3<GLdouble>            output_point;
        typedef std::vector<output_point>::iterator               output_iterator;

        template<typename BUFFER>
        void operator()(const input_polygon& _mp, scalar_type _z, BUFFER& _buffer)
        {
          drawSlice(_buffer.tess(),_mp,_z,_buffer.pointIterator());
        }
      private:
        template<class T, class P> void tessPolygon( T _tess, const P& _poly, scalar_type _z, output_iterator& _pit )
        {
          gluTessBeginContour(_tess);
          {
            for( auto& _p : _poly )
            {
              *_pit = output_point(_p.x(),_p.y(),_z);
              gluTessVertex(_tess, _pit->p(), _pit->p());
              _pit++;
            }
          }
          gluTessEndContour(_tess);
        }
        template<class T> void drawSlice( T _tess, const input_polygon& _mp, scalar_type _z, output_iterator& _pit )
        {
          for ( auto& _p : _mp)
          {
            gluTessBeginPolygon(_tess, NULL);
            {
              gluTessNormal(_tess,0,0,1);
              tessPolygon(_tess,_p.boundary(),_z,_pit);
              for( auto& _h : _p.holes() )
                tessPolygon(_tess,_h,_z,_pit);
            }
            gluTessEndPolygon(_tess);
          }
        }
      };
    }
  }
}

