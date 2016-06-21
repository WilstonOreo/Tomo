#include <tomo/query/Inquirer.hpp>
#include "query/Path.hpp"
#include "item/SliceGraph.hpp"
#include "query/Slice.hpp"

namespace tomo
{
  namespace fdm
  {
    namespace query
    {
      class SliceGraph : 
        public Inquirer<SliceGraph,item::SliceGraph>
      {
        TBD_PARAMETER_LIST
        (
          (bool) slice_information,
          (bool) path_information
        )

      public:
        TOMO_INQUIRER(item::SliceGraph)

        SliceGraph(base::Element const* _element) :
          Inquirer<SliceGraph,item::SliceGraph>(_element),
          slice_information_(true),
          path_information_(true) {}

        void preprocess()
        {
          auto& _sliceGraph = this->get()->data();
          data().clear();
          slicesQuery_(_sliceGraph);
          pathsQuery_(_sliceGraph);
        }

        void process()
        {
          typedef tbd::ConfigPath path_type;
          if (slice_information())
            slicesQuery_.save(path_type("slices"),data());
          if (path_information())
            pathsQuery_.save(path_type("paths"),data());
        }

      private:
        query::Slices slicesQuery_;
        query::Paths pathsQuery_;
      };
    }
  }
}

TOMO_INQUIRER_PLUGIN_INTERFACE(tomo::fdm::query::SliceGraph)
