#pragma once

#include <tomo/base/debug.hpp>
#include <gex/algorithm/boost.hpp>
#include <gex/algorithm/join.hpp>
#include <gex/algorithm/convert.hpp>
#include "Regular.hpp"
#include "SegmentGenerator.hpp"
#include "SliceGenerator.hpp"

namespace tomo
{
  namespace slice
  {
    namespace sampling
    {
      template<
        typename INPUT,
        typename SAMPLE_TYPE,
        typename SLICEGRAPH>
      struct Sampler
      {
      };

      template<typename INPUT,typename SLICE>
      struct Sampler<INPUT,Regular<>,SLICE>
      {
        typedef Regular<> sample_type;
        
        template<typename SAMPLES, typename TRANSFORMATION, typename SIMPLIFY, typename SLICEGRAPH>
        void operator()(const INPUT& _input,
                        const TRANSFORMATION& _transform,
                        const SIMPLIFY& _simplify,
                        SAMPLES& _samples,
                        SLICEGRAPH& _sliceGraph)
        {
          if (_input.empty()) return;

          typedef typename INPUT::scalar_type scalar_type;
          // Calculate intersections between sampling points and mesh's triangles
          // and insert resulting segments
          SegmentGenerator<INPUT,sample_type> _segmentGenerator;
          _segmentGenerator(_input,_transform,_samples);
          
          auto _diagonalLength = _input.bounds().size().norm(); 
          
          auto _epsilon = (_diagonalLength * _diagonalLength) / 10e20;
          auto _maxDist = _diagonalLength / 10e02;

          typedef gex::Point2 point_type;
          typedef gex::prim::MultiLineString<point_type> linestrings_type;
          
          size_t _numDefectSliceSegments = 0;
          for (const auto& _sample : _samples)
          {
            linestrings_type _lineStrings;
            // Connect segments to polygons
            gex::MultiPolygon _polygons;
            gex::join(_sample.segments(),_polygons,gex::strategy::join::DefectRings<point_type>(_lineStrings,_epsilon,_maxDist));
            TOMO_DBG(_polygons.size() << " " << _sample.segments().size())
            gex::algorithm::correct(_polygons);

            for (auto& _polygon : _polygons)
            {
              gex::Polygon _simplePolygon;
              gex::algorithm::simplify(_polygon,_simplePolygon,_maxDist / 10e2 + _simplify);
              _simplePolygon.update();
              _sliceGraph.insert(SLICE(_sample.range(),_simplePolygon));
              _sliceGraph.insertDefect(_sample.range().middle(),_lineStrings);
            }
            _numDefectSliceSegments += _lineStrings.size();
          }

          TOMO_DBG(_numDefectSliceSegments << " defect segments.")
        }
      };
    }
  }
}
