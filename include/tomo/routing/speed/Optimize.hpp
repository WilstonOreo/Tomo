#pragma once

namespace tomo
{
  namespace routing
  {
    namespace strategy
    {
      using gex::Range;
      using gex::Scalar;

      struct Optimize
      {
        Optimize(const Range& _speedRange, 
        Scalar _step = 5.0) :
          speedRange_(_speedRange),
          step_(_step) {}

        template<typename SPEED>
        SPEED quantize(const SPEED& _speed) const
        {
          return round(_speed / step()) * step();
        }

        TBD_PROPERTY_REF(Range,speedRange)
        TBD_PROPERTY_REF(Scalar,step)
      };
    }
  }
}
