#include <tomo/query/Inquirer.hpp>
#include "item/Route.hpp"
#include "routing/Extruder.hpp"
#include "query/Path.hpp"

namespace tomo
{
  namespace fdm
  {
    namespace query
    {
      class Route : 
        public Inquirer<Route,item::Route>
      {
        TBD_PARAMETER_LIST
        (
          (bool) path_information,
          (bool) state_information
        )
      public:
        TOMO_INQUIRER(item::Route)

        Route(base::Element const* _element) :
          Inquirer<Route,item::Route>(_element),
          path_information_(true),
          state_information_(true) {}

        void preprocess()
        {
          fdm::Route const& _route = this->get()->data();
          data().clear();
          pathQuery_(_route);

          using namespace tomo::routing;          
          float _travelLength = 0.0;
          State _finalState;
          StateIncrementer<> _inc;

          iterate(_route,tomo::routing::strategy::StateWithIncrementer<StateIncrementer<>>(_inc),[&](const State& _prev, const State& _next)
          {
            _finalState = _next;
            if (_prev.travel())
            {
              _travelLength += gex::distance(_prev,_next);
              TOMO_DBG(_travelLength)
            }
          });

          filamentLength_ = _finalState.filament_length(); 
          travelLength_ = _travelLength;
        }

        void process()
        {
          typedef tbd::ConfigPath path_type;
          if (path_information())
            pathQuery_.save(path_type("path"),data());
          data().put(path_type("filament_length"),filamentLength_); 
          data().put(path_type("travel_length"),travelLength_); 
        }

      private:
        float filamentLength_;
        float travelLength_;
        query::Path pathQuery_;
      };
    }
  }
}

TOMO_INQUIRER_PLUGIN_INTERFACE(tomo::fdm::query::Route)

