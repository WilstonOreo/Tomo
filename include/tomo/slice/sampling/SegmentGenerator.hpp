#pragma once

#include "Regular.hpp"
#include <gex/comp.hpp>
#include <gex/algorithm/util.hpp>

namespace tomo
{
  namespace slice
  {
    namespace sampling
    {
      template<typename INPUT, typename SAMPLE_TYPE>
      struct SegmentGenerator
      {
      };

      template<>
      struct SegmentGenerator<gex::TriangleMesh,Regular<gex::Z>> 
      { 
        typedef gex::TriangleMesh input_type;
        static gex::Axis constexpr axis() { return Regular<gex::Z>::axis(); }
       
        template<typename TRANSFORMATION, typename SAMPLES>
        void operator()(
            const input_type& _mesh, 
            const TRANSFORMATION& _transform, 
            SAMPLES& _samples) 
        {
          for (auto& _tri : _mesh.triangles())
          {
            auto N = _transform.rotation() * _tri.normal();
            auto A = _transform * _tri.v0(),
                 B = _transform * _tri.v1(),
                 C = _transform * _tri.v2();
            // Sort vertices and corresponding lineSegmentPlane iterators,
            // So that A is the lower, B in the middle and C the upper vertex
            //       /C
            //      / |
            //      B |
            //      \ |
            //       \A
            if (A[axis()] > B[axis()]) std::swap(A,B);
            if (B[axis()] > C[axis()]) std::swap(B,C);
            if (A[axis()] > B[axis()]) std::swap(A,B);
            sliceTriangle(A,B,C,N,_samples);
          }
        }

      private:
        template<typename POINT, typename VEC, typename SAMPLES>
        void sliceTriangle(const POINT& A,
                           const POINT& B,
                           const POINT& C,
                           const VEC& N,
                           SAMPLES& _samples) 
        {
          typedef POINT point_type;
          typedef typename point_type::Scalar scalar_type;

          auto _Ait = std::lower_bound(_samples.begin(),_samples.end(),A[axis()]),
               _Bit = std::lower_bound(_samples.begin(),_samples.end(),B[axis()]),
               _Cit = std::lower_bound(_samples.begin(),_samples.end(),C[axis()]);

          auto b = B - A, 
               c = C - A, 
               d = C - B;
          scalar_type _invBz = 1.0 / b[axis()];
          scalar_type _invCz = 1.0 / c[axis()];
          scalar_type _invDz = 1.0 / d[axis()];
          scalar_type _ratioR, _ratioS, _pos;

          using namespace gex::base;

          bool _passedB = false;
          constexpr gex::Axis _u = Axis(int(axis()+1) % 3),
                              _v = Axis(int(axis()+2) % 3);
          gex::Vec2 r,s, _Aproj;

          ///@todo make this generic
          _Aproj(X) = A(X);
          _Aproj(Y) = A(Y);

          for (auto it = _Ait ; it != _Cit && it != _samples.end(); ++it)
          {
            auto& _sample = *it;
            scalar_type _zPos = _sample.samplingPoint();

            if (it == _Bit) _passedB = true;

            if (!_passedB)
            {
              _pos = _zPos - A.z();
              _ratioR = _pos * _invCz;
              _ratioS = _pos * _invBz;
              s(X) = b.x()*_ratioS;
              s(Y) = b.y()*_ratioS;
            }
            else
            {
              _pos = _zPos;
              _ratioR = (_pos - A[axis()]) * _invCz;
              _ratioS = (_pos - B[axis()]) * _invDz;
              s(X) = b[_u] + d[_u]*_ratioS;
              s(Y) = b[_v] + d[_v]*_ratioS;
            }

            r(X) = c[_u]*_ratioR;
            r(Y) = c[_v]*_ratioR;

            gex::Point2 _p0 = _Aproj+r, _p1 = _Aproj+s;
            auto _normal = gex::algorithm::util::getNormal(_p0,_p1);  // Normal of segment points
            if (_normal.squaredNorm() == 0.0) continue;
            // Swap point to assure that points are in proper order to be able to 
            // connect segments later on
            if (gex::Vec2(N[_u],N[_v]).dot(_normal) < 0.0)
              _sample.addSegment(_p1,_p0);
            else
              _sample.addSegment(_p0,_p1);
          }
        }
      };
    }
  }
}
