#pragma once 

#include "../item/SliceGraph.hpp"

namespace tomo
{
  namespace fdm
  {
    namespace query
    {
      class Slices : public Serializer<Slices>
      {
        TBD_PARAMETER_LIST
        (
          (int) num_slices,
          (int) num_defect_slices
        )
      public:
        Slices() : Serializer<Slices>(*this) {}
        void operator()(const fdm::SliceGraph& _sliceGraph)
        {
          num_slices_ = _sliceGraph.slices().size();
          num_defect_slices_ = 0;
        }
      }; 
    }
  }
}
