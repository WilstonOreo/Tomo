#pragma once

#include "Draw.hpp"
#include <gex/prim.hpp>
#include "detail.hpp"
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

namespace tomo
{
  namespace gl
  {
#define TOMO_GL_STRINGIZE_DETAIL(x) #x
#define TOMO_GL_STRINGIZE(x) TOMO_GL_STRINGIZE_DETAIL(x)
#define TOMO_GL_CHECK(x) check(__FILE__ ":" TOMO_GL_STRINGIZE(__LINE__), x)

    template<typename SCALAR>
    struct Draw<gex::prim::Polygon<SCALAR>>
    {
      typedef SCALAR                                      scalar_type;
      typedef gex::base::Range<scalar_type>    range_type;
      typedef gex::prim::Polygon<scalar_type> input_polygon;
      typedef gex::prim::Ring<scalar_type>    ring_type;
      typedef gex::base::Point<gex::base::Model<3,GLdouble>>      tess_point;
      typedef std::vector<tess_point>::iterator           tess_iterator;
      typedef typename input_polygon::size_type           size_type;
      typedef gex::prim::Segment<scalar_type> segment_type;

      void operator()(const input_polygon& _p, scalar_type _z)
      {
        outline(_p,_z);
      }
      template<typename BUFFER>
      void operator()(const input_polygon& _p, const range_type& _z, BUFFER& _buffer)
      {
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        glShadeModel(GL_FLAT);
        edge(_p,_z);
        glShadeModel(GL_SMOOTH);
        TOMO_ASSERT(_buffer.tess() != 0);
        gluTessNormal(_buffer.tess(),0,0,-1);
        tesselate(_buffer.tess(),_p,_z.min(),_buffer.pointIterator());
        gluTessNormal(_buffer.tess(),0,0,1);
        tesselate(_buffer.tess(),_p,_z.max(),_buffer.pointIterator());
      }

      size_type countTess(const input_polygon& _p)
      {
        return 2*_p.countPoints();
      }
    private:
      static GLenum scalartype2glenum()
      {
        return detail::type2glenum<scalar_type>();
      }
      bool check(const char* _source, const ring_type& _ring)
      {
        if(_ring.front() != _ring.back())
        {
          std::cout.precision(15);
          std::cerr << _source << ": warning: unclosed ring: " << _ring.front() << " != " << _ring.back() << std::endl;
          glColor4f(1,0,0,1);
          return true;
        }
        else
          return false;
      }

      template<class T>
      void contour( T _tess, const ring_type& _ring, scalar_type _z, tess_iterator& _pit )
      {
        detail::glNormal(0,0,1);
        TOMO_GL_CHECK(_ring);
        gluTessBeginContour(_tess);
        {
          for( auto _it=_ring.begin()+1; _it!=_ring.end(); _it++ )
          {
            *_pit = tess_point(_it->x(),_it->y(),_z);
            gluTessVertex(_tess, _pit->p(), _pit->p());
            _pit++;
          }
        }
        gluTessEndContour(_tess);
      }
      template<class T> void tesselate( T _tess, const input_polygon& _poly, scalar_type _z, tess_iterator& _pit )
      {
        gluTessBeginPolygon(_tess, NULL);
        {
          contour(_tess,_poly.boundary(),_z,_pit);
          for( auto& _h : _poly.holes() )
            contour(_tess,_h,_z,_pit);
        }
        gluTessEndPolygon(_tess);
      }

      void outline( const input_polygon& _poly, scalar_type _z )
      {
        glTranslatef(0,0,_z);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(input_polygon::dimensions(),scalartype2glenum(),0,_poly.boundary().data());
        glDrawArrays(GL_LINE_LOOP, 0, _poly.boundary().size());

        for( auto& _h : _poly.holes() )
        {
          glVertexPointer(input_polygon::dimensions(),scalartype2glenum(),0,_h.data());
          glDrawArrays(GL_LINE_LOOP, 0, _h.size());
        }
        glDisableClientState(GL_VERTEX_ARRAY);
        glTranslatef(0,0,-_z);
      }

      void contour( const ring_type& _ring, range_type _z )
      {
        TOMO_GL_CHECK(_ring);
        glBegin(GL_QUAD_STRIP);
        {
          segment_type _seg(_ring.back(),_ring.back());
          for( auto& _p : _ring )
          {
            _seg[0] = _seg[1];
            _seg[1] = _p;
            auto&& _n = _seg.normal();
            detail::glNormal(_n.x(),_n.y(),scalar_type(0));
            detail::glVertex(_p.x(),_p.y(),_z.max());
            detail::glVertex(_p.x(),_p.y(),_z.min());
          }
        }
        glEnd();
      }
      void edge( const input_polygon& _poly, range_type _z )
      {
        contour(_poly.boundary(),_z);
        for( auto& _h : _poly.holes() )
          contour(_h,_z);
      }
    };
  }
}


