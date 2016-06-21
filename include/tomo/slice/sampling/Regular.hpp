#pragma once

#include <gex/prim.hpp>
#include <gex/algorithm/intersection.hpp>
#include <tomo/base/assert.hpp>

namespace tomo
{
  namespace slice
  {
    namespace sampling
    {
      template<gex::Axis AXIS = gex::Z>
      struct Regular
      {
        static constexpr gex::Axis axis() { return AXIS; }
      
        typedef gex::Segment segment_type;
        typedef gex::MultiSegment segment_cntr_type;
        typedef gex::Range range_type;
        typedef gex::Scalar scalar_type;

        Regular(range_type _range) : range_(_range) {}

        scalar_type samplingPoint() const
        {
          return range_.middle();
        }

        template<typename POINT>
        void addSegment(const POINT& _p0, const POINT& _p1)
        {
          segments_.push_back(segment_type(_p0,_p1));
        }

        friend bool operator<(const Regular& _lhs, const Regular& _rhs)
        {
          return _lhs.samplingPoint() < _rhs.samplingPoint();
        }

        friend bool operator<(const Regular& _lhs, const Regular::scalar_type _rhs)
        {
          return _lhs.samplingPoint() < _rhs;
        }

        TBD_PROPERTY(range_type,range)
        TBD_PROPERTY_REF(segment_cntr_type,segments)
      };

      template<typename INPUT, typename SAMPLE_TYPE>
      struct SampleGenerator
      {
      };

      template<typename INPUT>
      struct SampleGenerator<INPUT,Regular<>>
      {
        template<typename RANGE, typename SAMPLES>
        void operator()(
                        const RANGE& _range,
                        typename RANGE::scalar_type _firstLayerHeight,
                        typename RANGE::scalar_type _sampleHeight,
                        SAMPLES& _samples)
        {
          TOMO_ASSERT(_sampleHeight > 0);
          typedef typename INPUT::scalar_type scalar_type;
          typedef typename SAMPLES::value_type sample_type;
          typedef typename sample_type::range_type range_type;

          /// Get range in AXIS direction
          range_type _sampleRange(_range.min(),_range.min() + _firstLayerHeight);
          _samples.emplace_back(_sampleRange);
          _sampleRange(_range.min() + _firstLayerHeight,_range.min() + _firstLayerHeight + _sampleHeight);

          /// Determine number of samples
          size_t _numSamples = size_t(_range.length() / _sampleHeight) + 1;
          _samples.reserve(_numSamples);

          /// Calculate each sample
          while (_sampleRange.max() <= _range.max())
          {
            _samples.emplace_back(_sampleRange);
            _sampleRange += _sampleHeight;
          }
        }
      };
    }
  }
}
