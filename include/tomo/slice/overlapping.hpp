#pragma once

#include <tbd/boolean.h>

#include <gex/prim.hpp>
#include <gex/algorithm/boost.hpp>
#include <gex/algorithm/convert.hpp>
#include <gex/algorithm/intersects.hpp>
#include <gex/algorithm/offset.hpp>
#include <gex/polygon.hpp>
#include "traversal.hpp"

namespace tomo
{
  namespace slice
  {
    namespace strategy
    {
      namespace
      {
        struct OffsetAngle
        {
          OffsetAngle(float _offsetAngle) : 
            offsetAngle_(_offsetAngle) {}
          TBD_PROPERTY_RO(float,offsetAngle)
        };

        struct ShellHeight 
        {
          ShellHeight(float _shellHeight) :
            shellHeight_(_shellHeight) {}
          TBD_PROPERTY_RO(float,shellHeight)
        };

        struct Inverse
        {
          Inverse(gex::MultiPolygon& _inverse) :
            inverse_(_inverse) {}

          gex::MultiPolygon& inverse() { return inverse_; }
          gex::MultiPolygon const & inverse() const { return inverse_; }

        private:
          gex::MultiPolygon& inverse_; 
        };
      }

      struct TopBottom {};

      struct Top_Offset : OffsetAngle 
      {
        Top_Offset(float _offsetAngle) :
          OffsetAngle(_offsetAngle) {}
      };

      struct Bottom_Offset : OffsetAngle 
      {
        Bottom_Offset(float _offsetAngle) :
          OffsetAngle(_offsetAngle) {}
      };


      struct Top_Height : ShellHeight
      {
        typedef strategy::Top traversal_type;

        explicit Top_Height(float _shellHeight) : 
          ShellHeight(_shellHeight) {}
      };
      
      struct Bottom_Height : ShellHeight
      {
        typedef strategy::Bottom traversal_type;
        
        explicit Bottom_Height(float _shellHeight) : 
          ShellHeight(_shellHeight) {}
      };
      
      struct TopBottom_Height : ShellHeight
      {     
        explicit TopBottom_Height(float _shellHeight) : 
          ShellHeight(_shellHeight) {}
      };
      
      struct Top_Height_Inv : Top_Height, Inverse
      {
        typedef strategy::Top traversal_type;
        
        explicit Top_Height_Inv(float _shellHeight, gex::MultiPolygon& _inverse) : 
          Top_Height(_shellHeight),
          Inverse(_inverse) {}
      };
      
      struct Bottom_Height_Inv : Bottom_Height, Inverse
      {
        typedef strategy::Bottom traversal_type;
        
        explicit Bottom_Height_Inv(float _shellHeight, gex::MultiPolygon& _inverse) : 
          Bottom_Height(_shellHeight),
          Inverse(_inverse) {}
      };

      struct TopBottom_Height_Inv : TopBottom_Height, Inverse
      {
        explicit TopBottom_Height_Inv(float _shellHeight, gex::MultiPolygon& _inverse) :
          TopBottom_Height(_shellHeight),
          Inverse(_inverse) {}
      };
    }

    namespace functor
    {
      template<typename SLICESET, typename SLICEGRAPH, typename SLICE>
      gex::MultiPolygon overlapPolygons(const SLICEGRAPH& _graph, const SLICE& _slice)
      {
        using namespace gex;
        auto&& _slices = strategy::getSliceSet<SLICESET>(_graph,_slice);
        auto&& _bounds = _slice.polygon().bounds();
        polygon::MultiPolygon _polygons;
        for (auto& _overlapSlice : _slices)
        {
          using namespace boost::polygon::operators;
          _polygons += polygon::adapt(_overlapSlice->polygon(),_bounds);
        }
        return polygon::adapt(_polygons,_bounds);
      }

      template<typename SLICES, typename BOUNDS, typename POLYGON>
      void polygonSliceDiff(const SLICES& _slices, const BOUNDS& _bounds, POLYGON& _polygon)
      {
        using namespace gex;
        for (auto& _slice : _slices)
        {
          using namespace boost::polygon::operators;
          _polygon -= polygon::adapt(_slice->polygon(),_bounds);
          if (_polygon.empty()) break;
        }
      }

      template<typename SLICE, typename IN, typename OUT>
      void sliceDifference(const SLICE& _slice, const IN& _in, OUT& _out)
      {
        using namespace gex; 

        auto& _bounds = _slice.polygon().bounds();
        auto&& _rest = polygon::adapt(convert<MultiPolygon>(_slice.polygon()),_bounds);
        polygonSliceDiff(_in,_bounds,_rest);
        _out = polygon::adapt(_rest,_bounds);
        _out.update();
      };

      template<typename SLICE, typename IN, typename OUT>
      void sliceDifference(const SLICE& _slice, const IN& _in, OUT& _out, OUT& _inverse)
      {
        using namespace gex; 
        auto& _bounds = _slice.polygon().bounds();
        polygon::MultiPolygon&& _polygon = polygon::adapt(convert<MultiPolygon>(_slice.polygon()),_bounds);
        auto _rest = _polygon;
        polygonSliceDiff(_in,_bounds,_rest);

        using namespace boost::polygon::operators;
        polygon::MultiPolygon&& _diff = _polygon - _rest;
        _inverse = polygon::adapt(_diff,_bounds);
        _inverse.update();

        _out = polygon::adapt(_rest,_bounds); 
        _out.update();
      };

      template<typename SLICE, typename LOWER, typename UPPER, typename OUT>
      void sliceDifferenceAndUnion(const SLICE& _slice, 
          const LOWER& _lower, 
          const UPPER& _upper,
          OUT& _out) 
      {
        using namespace gex;

        auto& _bounds = _slice.polygon().bounds();
        polygon::MultiPolygon&& _polygon = polygon::adapt(convert<MultiPolygon>(_slice.polygon()),_bounds); 
        auto _lowerRest = _polygon;
        auto _upperRest = _polygon;
        polygonSliceDiff(_lower,_bounds,_lowerRest);
        polygonSliceDiff(_upper,_bounds,_upperRest);

        using namespace boost::polygon::operators;
        polygon::MultiPolygon&& _rest = _lowerRest + _upperRest;
        _out = polygon::adapt(_rest,_bounds); 
        _out.update();
      }

      template<typename SLICE, typename LOWER, typename UPPER, typename OUT>
      void sliceDifferenceAndUnion(const SLICE& _slice, 
          const LOWER& _lower, 
          const UPPER& _upper,
          OUT& _out, OUT& _inverse) 
      {
        using namespace gex;

        auto& _bounds = _slice.polygon().bounds();
        polygon::MultiPolygon&& _polygon = polygon::adapt(convert<MultiPolygon>(_slice.polygon()),_bounds); 
        auto _lowerRest = _polygon;
        auto _upperRest = _polygon;
        polygonSliceDiff(_lower,_bounds,_lowerRest);
        polygonSliceDiff(_upper,_bounds,_upperRest);

        using namespace boost::polygon::operators;
        polygon::MultiPolygon&& _rest = _lowerRest + _upperRest;
        polygon::MultiPolygon&& _diff = _polygon - _rest;
        _inverse = polygon::adapt(_diff,_bounds);
        _inverse.update();
        _out = polygon::adapt(_rest,_bounds); 
        _out.update();
      }

      template<typename SLICE, typename STRATEGY>
      struct Overlapping {};

      template<typename SLICE>
      struct Overlapping<SLICE,strategy::Top>
      {
        typedef gex::MultiPolygon result_type;

        template<typename SLICEGRAPH, typename STRATEGY>
        void operator()(const SLICEGRAPH& _graph, const SLICE& _slice, STRATEGY _strategy, result_type& _out)
        {
          sliceDifference(_slice,_graph.indexesToPointers(
                strategy::getSliceSet<STRATEGY>(_slice)),_out);
        }
      };
      
      template<typename SLICE>
      struct Overlapping<SLICE,strategy::Bottom> : 
        Overlapping<SLICE,strategy::Top> 
      {
        typedef gex::MultiPolygon result_type;
      };

      template<typename SLICE>
      struct Overlapping<SLICE,strategy::Top_Offset>
      {
        typedef gex::MultiPolygon result_type;

        template<typename SLICEGRAPH, typename STRATEGY>
        void operator()(const SLICEGRAPH& _graph, const SLICE& _slice, STRATEGY _strategy, result_type& _out)
        {
          result_type _noOffset;
          sliceDifference(_slice,
              _graph.indexesToPointers(strategy::getSliceSet<STRATEGY>(_slice)),_noOffset);     
          gex::Scalar _offset = _slice.range().length() * tan(gex::deg2rad(_strategy.offsetAngle()));
          gex::offset(_noOffset,_offset,_out);
        }
      };
      
      template<typename SLICE>
      struct Overlapping<SLICE,strategy::Bottom_Offset> : 
        Overlapping<SLICE,strategy::Top_Offset> 
      {
        typedef gex::MultiPolygon result_type;
      };


      template<typename SLICE>
      struct Overlapping<SLICE,strategy::TopBottom> 
      {
        typedef gex::MultiPolygon result_type;

        template<typename SLICEGRAPH, typename STRATEGY>
        void operator()(const SLICEGRAPH& _graph, const SLICE& _slice, STRATEGY _strategy, result_type& _out)
        {
          sliceDifferenceAndUnion(_slice,
              _graph.indexesToPointers(_slice.lower()),
              _graph.indexesToPointers(_slice.upper()),_out);
        }
      };

      template<typename SLICE>
      struct Overlapping<SLICE,strategy::Top_Height>
      {
        typedef gex::MultiPolygon result_type;

        template<typename SLICEGRAPH, typename STRATEGY>
        void operator()(const SLICEGRAPH& _graph, const SLICE& _slice, STRATEGY _strategy, result_type& _out)
        {
          typedef typename SLICEGRAPH::sliceset_const_type sliceset_type; 
          sliceset_type _capSlices;
          typedef typename STRATEGY::traversal_type traversal_strategy_type;
          traversal(_graph,_slice,traversal_strategy_type(),[&](const sliceset_type& _upperSlices)
          {
            for_each(_upperSlices,[&](SLICE const* _upper)
            {
              if (strategy::rangeIntersects<traversal_strategy_type>(_slice,_strategy.shellHeight(),*_upper))
              {
                _capSlices.insert(_upper);
              }
            });
          });
          sliceDifference(_slice,_capSlices,_out);
        }        
      };

      template<typename SLICE>
      struct Overlapping<SLICE,strategy::Bottom_Height> : 
        Overlapping<SLICE,strategy::Top_Height>
      {
        typedef gex::MultiPolygon result_type;
      };

      template<typename SLICE>
      struct Overlapping<SLICE,strategy::TopBottom_Height>
      {
        typedef gex::MultiPolygon result_type;

        template<typename SLICEGRAPH, typename STRATEGY>
        void operator()(const SLICEGRAPH& _graph, const SLICE& _slice, STRATEGY _strategy, result_type& _out)
        {
          typedef typename SLICEGRAPH::sliceset_const_type sliceset_type; 
          sliceset_type _lowerCapSlices, _upperCapSlices;
          traversal(_graph,_slice,strategy::Top(),[&](const sliceset_type& _upperSlices)
          {
            for_each(_upperSlices,[&](SLICE const* _upper)
            {
              if (strategy::rangeIntersects<strategy::Top>(_slice,_strategy.shellHeight(),*_upper))
              {
                _upperCapSlices.insert(_upper);
              }
            });
          });

          traversal(_graph,_slice,strategy::Bottom(),[&](const sliceset_type& _lowerSlices)
          {
            for_each(_lowerSlices,[&](SLICE const* _lower)
            {
              if (strategy::rangeIntersects<strategy::Bottom>(_slice,_strategy.shellHeight(),*_lower))
              {
                _lowerCapSlices.insert(_lower);
              }
            });
          });
          sliceDifferenceAndUnion(_slice,_lowerCapSlices,_upperCapSlices,_out);
        }       
      };

      template<typename SLICE>
      struct Overlapping<SLICE,strategy::Top_Height_Inv>
      {
        typedef gex::MultiPolygon result_type;

        template<typename SLICEGRAPH, typename STRATEGY>
        void operator()(const SLICEGRAPH& _graph, const SLICE& _slice, STRATEGY _strategy, result_type& _out)
        {
          typedef typename SLICEGRAPH::sliceset_const_type sliceset_type; 
          sliceset_type _capSlices;
          typedef typename STRATEGY::traversal_type traversal_strategy_type;
          traversal(_graph,_slice,traversal_strategy_type(),[&](const sliceset_type& _upperSlices)
          {
            for_each(_upperSlices,[&](SLICE const* _upper)
            {
              if (strategy::rangeIntersects<traversal_strategy_type>(_slice,_strategy.shellHeight(),*_upper))
              {
                _capSlices.insert(_upper);
              }
            });
          });
          sliceDifference(_slice,_capSlices,_out,_strategy.inverse());
        }    
      };

      template<typename SLICE>
      struct Overlapping<SLICE,strategy::Bottom_Height_Inv> :
        Overlapping<SLICE,strategy::Top_Height_Inv> 
      {
        typedef gex::MultiPolygon result_type;
      };

      template<typename SLICE>
      struct Overlapping<SLICE,strategy::TopBottom_Height_Inv>
      {
        typedef gex::MultiPolygon result_type;

        template<typename SLICEGRAPH, typename STRATEGY>
        void operator()(const SLICEGRAPH& _graph, const SLICE& _slice, STRATEGY _strategy, result_type& _out)
        {
          typedef typename SLICEGRAPH::sliceset_const_type sliceset_type; 
          sliceset_type _lowerCapSlices, _upperCapSlices;
          traversal(_graph,_slice,strategy::Top(),[&](const sliceset_type& _upperSlices)
          {
            for_each(_upperSlices,[&](SLICE const* _upper)
            {
              if (strategy::rangeIntersects<strategy::Top>(_slice,_strategy.shellHeight(),*_upper))
              {
                _upperCapSlices.insert(_upper);
              }
            });
          });

          traversal(_graph,_slice,strategy::Bottom(),[&](const sliceset_type& _lowerSlices)
          {
            for_each(_lowerSlices,[&](SLICE const* _lower)
            {
              if (strategy::rangeIntersects<strategy::Bottom>(_slice,_strategy.shellHeight(),*_lower))
              {
                _lowerCapSlices.insert(_lower);
              }
            });
          });
          sliceDifferenceAndUnion(_slice,_lowerCapSlices,_upperCapSlices,_out,_strategy.inverse());
        } 
      };

    }

    using functor::Overlapping;

    template<typename SLICEGRAPH, typename SLICE, typename STRATEGY, typename RESULT>
    void overlapping(
        const SLICEGRAPH& _graph, 
        const SLICE& _slice, STRATEGY _strategy, RESULT& _result)
    {
      Overlapping<SLICE,STRATEGY>()(_graph,_slice,_strategy,_result);
    }

    template<typename SLICEGRAPH, typename SLICE, typename STRATEGY>
    typename Overlapping<SLICE,STRATEGY>::result_type overlapping(
        const SLICEGRAPH& _graph, const SLICE& _slice, STRATEGY _strategy)
    {
      typename Overlapping<SLICE,STRATEGY>::result_type _overlapping;
      overlapping(_graph,_slice,_strategy,_overlapping);
      return _overlapping;
    }
  }
}

