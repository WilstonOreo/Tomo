#pragma once

#include <gex/prim.hpp>
#include <gex/algorithm/boost.hpp>
#include <gex/algorithm/offset.hpp>
#include <gex/algorithm/perimeter.hpp>
#include <tomo/routing/generate.hpp>

#include <tomo/slice/for_each_parallel.hpp>

#include "../routing/MoveMaker.hpp"
#include "../item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    namespace strategy
    {
      namespace perimeter
      {
        struct Default
        {
          Default(int _number,
                  float _speed,
                  float _extrusionWidth,
                  float _pathTag) :
            number_(_number),
            speed_(_speed),
            extrusion_width_(_extrusionWidth),
            path_tag_(_pathTag)
          {
          }

          TBD_PROPERTY_RO(int,number)
          TBD_PROPERTY_RO(float,speed)
          TBD_PROPERTY_RO(float,extrusion_width)
          TBD_PROPERTY_RO(float,path_tag)
        };

        struct Inner
        {
          Inner(int _number, float _extrusionWidth) :
            number_(_number),
            extrusion_width_(_extrusionWidth)
          {
          }

          TBD_PROPERTY_RO(int,number)
          TBD_PROPERTY_RO(float,extrusion_width)
        };
      }
    }

    namespace functor
    {
      template<typename STRATEGY>
      struct Perimeter
      {
      };

      template<>
      struct Perimeter<strategy::perimeter::Default>
      {
        typedef fdm::SliceGraph output_type;

        template<typename IN, typename S, typename OUT>
        void operator()(const IN& _in, S _s, OUT& _out)
        {
          _out.clear();
          typedef tomo::slice::strategy::for_each::Parallel<fdm::SliceGraph> strategy_type;
          strategy_type _parallel(_out);

          for_each(_in,_parallel,
                   [&](const fdm::Slice& _slice) -> std::vector<fdm::Slice>
          {
            auto& _polygon = _slice.polygon();
            gex::MultiPolygon _perimeters;
            gex::MultiPolygon _offsetPolygons, _lastPolygons = gex::convert<gex::MultiPolygon>(_polygon);
            gex::Scalar _offset;

            for (int i = 0; i < _s.number(); i++)
            {
              _offset = - _s.extrusion_width()*(i+0.5);
              gex::offset(
                _polygon,
                gex::strategy::offset::Default(_offset), 
                _offsetPolygons);

              if (_offsetPolygons.empty()) break;

              for (auto& _p : _offsetPolygons)
              {
                _p.update();
                _perimeters.push_back(_p);
              }
              _lastPolygons = _offsetPolygons;
            }
            gex::MultiPolygon _simplified;
            gex::algorithm::simplify(_perimeters,_simplified,_s.extrusion_width()*0.1);

            for (auto& _p : _simplified)
              _p.update();

            Slice _perimeterSlice(_slice.range(),_slice.polygon(),_s.path_tag());
            routing::generate(_perimeters,fdm::MoveMaker(_slice,_s.speed(),_s.extrusion_width()),_perimeterSlice);
            return {_perimeterSlice};
          });
          _out.connect();
        }
      };

      template<>
      struct Perimeter<strategy::perimeter::Inner>
      {
        typedef fdm::SliceGraph output_type;

        template<typename IN, typename S, typename OUT>
        void operator()(const IN& _in, S _s, OUT& _out)
        {
          _out.clear();
          typedef tomo::slice::strategy::for_each::Parallel<fdm::SliceGraph> strategy_type;
          strategy_type _parallel(_out);
          for_each(_in,
                   _parallel,[&](const fdm::Slice& _slice) -> std::vector<fdm::Slice>
          {
            std::vector<fdm::Slice> _slices;
            gex::MultiPolygon _out;
            gex::offset(
              _slice.polygon(),
              gex::strategy::offset::Default(-_s.extrusion_width()*_s.number()),
              _out);
            for (auto& _p : _out)
            {
              _p.update();
              if (_p.boundary().empty()) continue;
              _slices.emplace_back(_slice.range(),_p);
            }
            return _slices;
          });
          _out.connect();
        }
      };
    }

    template<typename IN, typename STRATEGY, typename OUT>
    void perimeter(const IN& _in, STRATEGY _s, OUT& _out)
    {
      functor::Perimeter<STRATEGY>()(_in,_s,_out);
    }

    template<typename IN, typename STRATEGY>
    typename functor::Perimeter<STRATEGY>::output_type
    perimeter(const IN& _in, STRATEGY _s)
    {
      typename functor::Perimeter<STRATEGY>::output_type _out;
      perimeter(_in,_s,_out);
      return _out;
    }
  }
}
