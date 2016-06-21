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
        class Solid : public Visual
        {
        public:
          Solid(fdm::SliceGraph const* _sliceGraph = nullptr) :
            Visual(_sliceGraph),
            tess_(nullptr) {}

          ~Solid()
          {
            if (tess_)
              gluDeleteTess(tess_);
          }

          void update()
          {
            auto&& _slices = sliceGraph()->fetch();
            int _numPoints = 0;
            for (auto& _slice : _slices)
            {
              _numPoints += countTess(_slice->polygon());
            }
            
            displayLists_.free();
            displayLists_.initialize(_slices.size());
            points_.resize(_numPoints);

            if( _numPoints > 0 )
            {
              tess_ = gluNewTess();
              {
                gluTessCallback(tess_, GLU_TESS_VERTEX, (GLvoid (*) ()) glVertex3dv);
                gluTessCallback(tess_, GLU_TESS_BEGIN, (GLvoid (*) ()) glBegin);
                gluTessCallback(tess_, GLU_TESS_END, (GLvoid (*) ()) glEnd);
                gluTessProperty(tess_, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
              }
            }
            pointIterator_ = points_.begin();

            tbd::gl::compile_lists(displayLists_,[&](GLuint i)
            {
              auto& _slice = _slices[i];
              TOMO_DBG(_slice->range().middle())
              auto& _p = _slice->polygon();
              auto& _z = _slice->range();
              glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
              glShadeModel(GL_FLAT);
              edge(_p,_z);
              glShadeModel(GL_SMOOTH);
              TOMO_ASSERT(tess_ != 0);
              gluTessNormal(tess_,0,0,-1);
              tesselate(tess_,_p,_z.min(),pointIterator_);
              gluTessNormal(tess_,0,0,1);
              tesselate(tess_,_p,_z.max(),pointIterator_);
            });
          }

          void draw() 
          {
            using namespace tomo::slice;

            glEnable(GL_LIGHTING);
            displayLists_.call();
          }

          void draw(const gex::Range& _range) 
          {
            using namespace tomo::slice;
            pointIterator_ = points_.begin();

            auto&& _slices = sliceGraph()->fetch();
            std::vector<GLuint> _indices;
            _indices.reserve(_slices.size());
            for (GLuint i = 0; i < _slices.size(); ++i)
            {
              auto& _z = _slices[i]->range(); 
              if (!gex::intersects(_z,_range)) continue;
              _indices.push_back(i);
            }

            glEnable(GL_LIGHTING);
            displayLists_.call(_indices);
          }


        private:
          tbd::gl::DisplayLists displayLists_;
          typedef gex::base::Point<gex::base::Model<3,GLdouble>>      tess_point;
          typedef std::vector<tess_point>::iterator           tess_iterator;
           typedef gex::base::Model<3,GLdouble> model_type;
            typedef gex::base::Point<model_type> point_type;
            typedef std::vector<point_type>           point_vector;
            typedef typename point_vector::iterator   point_iterator;
          GLUtesselator* tess_;
          point_vector points_;
          point_iterator pointIterator_;

          template<typename POLYGON>
          size_t countTess(const POLYGON& _p)
          {
            return 2*_p.countPoints();
          }

          template<class T, class RING, class Z>
          void contour( T _tess, const RING& _ring, Z _z, tess_iterator& _pit )
          {
            using namespace tomo::visual;
            gl::normal(0.0f,0.0f,1.0f);
            gluTessBeginContour(_tess);
            {
              for( auto _it=_ring.begin()+1; _it!=_ring.end(); _it++ )
              {
                *_pit = tess_point(_it->x(),_it->y(),_z);
                gluTessVertex(_tess, _pit->data(), _pit->data());
                _pit++;
              }
            }
            gluTessEndContour(_tess);
          }
          template<class T, class POLYGON, class Z>
          void tesselate( T _tess, const POLYGON& _poly, Z _z, tess_iterator& _pit )
          {
            gluTessBeginPolygon(_tess, NULL);

            contour(_tess,_poly.boundary(),_z,_pit);
for( auto& _h : _poly.holes() )
              contour(_tess,_h,_z,_pit);
            gluTessEndPolygon(_tess);
          }

          template<typename RING, typename Z>
          void contour( const RING& _ring, Z _z )
          {
            typedef typename RING::scalar_type scalar_type;
            typedef typename RING::point_type point_type;

            gex::Segment _seg(_ring.back(),_ring.back());
            using namespace tomo::visual;
            
            gl::draw(GL_QUAD_STRIP,[&]()
            {
              for (auto& _p : _ring)
              {
                _seg[0] = _seg[1];
                _seg[1] = _p;
                auto&& _n = _seg.normal();
                gl::normal(_n.x(),_n.y(),scalar_type(0));
                gl::vertex(_p.x(),_p.y(),_z.max());
                gl::vertex(_p.x(),_p.y(),_z.min());
              }
            });
          }

          template<typename POLYGON, typename Z>
          void edge( const POLYGON& _poly, Z _z )
          {
            typedef typename POLYGON::ring_type ring_type;
            gex::for_each<ring_type>(_poly,[&](const ring_type& _r)
            {
              contour(_r,_z);
            });
          }
        };
      }
    }
  }
}
