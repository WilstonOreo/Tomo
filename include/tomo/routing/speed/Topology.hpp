#pragma once

#include <tomo/slice/Graph.hpp>
#include <tomo/slice/centroid.hpp>
#include <tomo/slice/traversal.hpp>
#include <tomo/slice/for_each.hpp>
#include <tomo/slice/overlapping.hpp>
#include <tomo/routing/bounds.hpp>
#include "Optimize.hpp"

namespace tomo
{
  namespace routing
  {
    namespace strategy
    {
      using gex::Range;
      using gex::Scalar;
      
      template<typename SLICE>
      struct Topology : Optimize
      {
        typedef SLICE slice_type;
        typedef slice::Graph<SLICE> slicegraph_type;
        typedef typename slice_type::sliceset_type sliceset_type;

        Topology(const slice::Graph<SLICE>& _sliceGraph, const Range& _speedRange,
            Scalar _step,
            Scalar _massFactor) :
          Optimize(_speedRange,_step),
          massFactor_(_massFactor),
          sliceGraph_(_sliceGraph)
        {
          mass_ = 0.0;
          slice::for_each(_sliceGraph,[&](const slice_type& _slice)
          {
            mass_ += _slice.range().length() * gex::algorithm::area(_slice.polygon());
          });
        }
      
        slicegraph_type const& sliceGraph() { return sliceGraph_; }

        template<typename PATH>
        sliceset_type getSlices(PATH const& _path)
        {
          auto&& _bounds = routing::bounds(_path);
          return sliceGraph_.query(gex::index::intersects(_bounds));
        }

        TBD_PROPERTY_RO(gex::Scalar,mass)
        TBD_PROPERTY(gex::Scalar,massFactor)

      private:
        slicegraph_type const& sliceGraph_;
      };
    }

    namespace functor
    {
      template<typename PATH, typename SLICE>
      struct SpeedOptimization<PATH,strategy::Topology<SLICE>>
      {
        template<typename STRATEGY>
        void operator()(const PATH& _path, STRATEGY _strategy, PATH& _opt)
        {
          typedef SLICE slice_type;
          typedef typename slice_type::sliceset_type sliceset_type;
          auto&& _slices = _strategy.getSlices(_path);
          if (_slices.empty())
          {
            _opt = _path;
            return;
          }
          
          gex::Scalar _mass = 0.0, _overlapMass = 0.0;

          slice::for_each(_slices,[&](slice_type const* _slice)
          {
            _mass += gex::algorithm::area(_slice->polygon())*_slice->range().length();
            auto&& _centroid = slice::centroid(*_slice);

            slice::traversal(*_slice,slice::strategy::Bottom(),[&](const sliceset_type& _lowerSlices)
            {
              slice::for_each(_lowerSlices,[&](slice_type const* _lowerSlice)
              {
                gex::Scalar _overlappingArea = gex::algorithm::area(slice::overlapping(*_lowerSlice,slice::strategy::Bottom()));
                auto& _range = _lowerSlice->range(); 
                auto _sqrDistance = 
                  gex::sqrDistance(_centroid,slice::centroid(*_lowerSlice));
                _mass += _range.length()*gex::algorithm::area(_lowerSlice->polygon()) / _sqrDistance ;
                _overlapMass += _range.length()*_overlappingArea / _sqrDistance;
              });
            });
          });

          typedef typename PATH::point_type move_type;
          for_each_move(_path,[&](const move_type& _move)
          {
            _opt.push_back(_move);
            
            auto& _speedRange = _strategy.speedRange();
            
            gex::Scalar _speed = _move.speed() * _overlapMass / _strategy.mass() * _strategy.massFactor();
            if (_speed < _speedRange.min()) 
              _speed = _speedRange.min();
            if (_speed > _speedRange.max()) 
              _speed = _speedRange.max();
            _opt.back().speed(_speed);
          });
        }
      };
    }
  }
}
