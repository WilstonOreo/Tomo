#pragma once

#include <boost/geometry/algorithms/intersection.hpp>
#include <gex/prim.hpp>
#include <gex/polygon.hpp>

namespace gex
{
    namespace algorithm
    {
      template<typename A, typename B>
      struct IntersectionType {};

#define GEX_INTERSECTION_TYPE(a,b,i) \
        template<> struct IntersectionType<a,b> { typedef i type; };

      GEX_INTERSECTION_TYPE(Ring,Ring,MultiPolygon)
      GEX_INTERSECTION_TYPE(Ring,Polygon,MultiPolygon)
      GEX_INTERSECTION_TYPE(Ring,MultiPolygon,MultiPolygon)
      GEX_INTERSECTION_TYPE(Polygon,MultiPolygon,MultiPolygon)
      GEX_INTERSECTION_TYPE(Polygon,Polygon,MultiPolygon)
      GEX_INTERSECTION_TYPE(Polygon,Ring,MultiPolygon)
      GEX_INTERSECTION_TYPE(MultiPolygon,Ring,MultiPolygon)
      GEX_INTERSECTION_TYPE(MultiPolygon,Polygon,MultiPolygon)
      GEX_INTERSECTION_TYPE(MultiPolygon,MultiPolygon,MultiPolygon)
      GEX_INTERSECTION_TYPE(Range,Range,Range)

      GEX_INTERSECTION_TYPE(OrthogonalPlane<X>,Triangle,Segment)
      GEX_INTERSECTION_TYPE(OrthogonalPlane<Y>,Triangle,Segment)
      GEX_INTERSECTION_TYPE(OrthogonalPlane<Z>,Triangle,Segment)

    GEX_INTERSECTION_TYPE(polygon::Ring,polygon::Ring,polygon::MultiPolygon)
    GEX_INTERSECTION_TYPE(polygon::Ring,polygon::Polygon,polygon::MultiPolygon)
    GEX_INTERSECTION_TYPE(polygon::Ring,polygon::MultiPolygon,polygon::MultiPolygon)
    GEX_INTERSECTION_TYPE(polygon::Polygon,polygon::Ring,polygon::MultiPolygon)
    GEX_INTERSECTION_TYPE(polygon::Polygon,polygon::Polygon,polygon::MultiPolygon)
    GEX_INTERSECTION_TYPE(polygon::Polygon,polygon::MultiPolygon,polygon::MultiPolygon)
    GEX_INTERSECTION_TYPE(polygon::MultiPolygon,polygon::Ring,polygon::MultiPolygon)
    GEX_INTERSECTION_TYPE(polygon::MultiPolygon,polygon::Polygon,polygon::MultiPolygon)
    GEX_INTERSECTION_TYPE(polygon::MultiPolygon,polygon::MultiPolygon,polygon::MultiPolygon)

/*
        bool intersect(const ray_type& _ray, scalar_type* _clampTMin = nullptr, scalar_type* _clampTMax = nullptr) const
        {
          scalar_type _tMin = _ray.tNear();
          scalar_type _tMax = _ray.tFar();
          GEX_FOREACH_DIM(i)
          {
            scalar_type _tMinDim = (min_[i] - _ray.org()[i]) / _ray.dir()[i];
            scalar_type _tMaxDim = (max_[i] - _ray.org()[i]) / _ray.dir()[i];
            if (_ray.dir()[i] < 0) std::swap(_tMinDim,_tMaxDim);
            if ((_tMin > _tMaxDim) || (_tMax > _tMinDim)) return false;
            std::max(_tMin,_tMinDim);
            std::min(_tMax,_tMaxDim);
          }
          if ((_tMin <= _ray.tNear()) && (_tMax >= _ray.tFar()))
          {
            if (_clampTMin) (*_clampTMin) = (_tMin > _ray.tNear()) ? _tMin : _ray.tNear();
            if (_clampTMax) (*_clampTMax) = (_tMax < _ray.tFar()) ? _tMax : _ray.tFar();
            return true;
          }
          return false;
        }
*/


      template<typename A, typename B>
      struct Intersection
      {
        template<typename I>
        void operator()(const A& _a, const B& _b, I& _i)
        {
          try 
          {
            boost::geometry::intersection(_a,_b,_i);
          }
          catch (std::exception e)
          {
            _i = I();
            std::cerr << "Warning: boost::geometry::intersection failed due to numerical inconsistencies." << std::endl;
          }
        }
      };

      /// Boost Polygon operations
      template<>
      struct Intersection<polygon::Polygon,polygon::Polygon>
      {
        template<typename A, typename B, typename I>
        inline void operator()(const A& _a, const B& _b, I& _i)
        {
          using namespace boost::polygon::operators;
          _i = _a ^ _b;
        }
      };
      
    template<>
    struct Intersection<polygon::Ring,polygon::Ring> :
        Intersection<polygon::Polygon,polygon::Polygon> {};
    
    template<>
    struct Intersection<polygon::Ring,polygon::Polygon> :
        Intersection<polygon::Polygon,polygon::Polygon> {};
    
    template<>
    struct Intersection<polygon::Ring,polygon::MultiPolygon> :
        Intersection<polygon::Polygon,polygon::Polygon> {};

    template<>
    struct Intersection<polygon::Polygon,polygon::Ring> :
        Intersection<polygon::Polygon,polygon::Polygon> {};

    template<>
    struct Intersection<polygon::Polygon,polygon::MultiPolygon> :
        Intersection<polygon::Polygon,polygon::Polygon> {};
    
    template<>
    struct Intersection<polygon::MultiPolygon,polygon::Ring> :
        Intersection<polygon::Polygon,polygon::Polygon> {};

    template<>
    struct Intersection<polygon::MultiPolygon,polygon::Polygon> :
        Intersection<polygon::Polygon,polygon::Polygon> {};

    template<>
    struct Intersection<polygon::MultiPolygon,polygon::MultiPolygon> :
        Intersection<polygon::Polygon,polygon::Polygon> {};      
      template<typename POINT>
      struct Intersection<prim::Segment<POINT>,prim::Segment<POINT>>
      {
      private:
       typedef POINT point_type;
        typedef prim::Segment<POINT> segment_type;

        template<typename PRECISION = double>
        bool operator()(
            const segment_type& _a, 
            const segment_type& _b, 
            point_type& _i, 
            PRECISION _eps = 0.0001)
        {
          return false;
     //     return util::lineSegmentIntersection<point_type,PRECISION>()(_a,_b,_iPoint,_eps);
        }
      };

      template<typename SCALAR>
      struct Intersection<base::Range<SCALAR>,base::Range<SCALAR>>
      {
        typedef base::Range<SCALAR> range_type;

        void operator()(const range_type& _a, const range_type& _b, range_type& _i)
        {
          _i.min(std::max(_a.min(),_b.min()));
          _i.max(std::min(_a.max(),_b.max()));
          _i.validate();
        }
      };
      
      template<typename A, typename B, typename I>
      void intersection(const A& _a, const B& _b, I& _i)
      {
        Intersection<A,B>()(_a,_b,_i);
      }

      template<typename A, typename B>
      typename IntersectionType<A,B>::type intersection(const A& _a, const B& _b)
      {
        typename IntersectionType<A,B>::type _i;
        intersection<A,B>()(_a,_b,_i);
        return _i;
      }

      template<typename A, typename B>
      typename IntersectionType<A,B>::type operator&(const A& _a, const B& _b)
      {
        return intersection(_a,_b);
      } 
    }
  }
