#pragma once

#include <functional>

namespace tomo
{
  namespace routing
  {
    /**@brief An event contains a function pointer and 
     *        a pointer to input data for this function
     * @tparam MACHINE_STATE State the machine can hold a certain point in time
    **/
    template<typename MACHINE_STATE>
    struct Event 
    {
      typedef MACHINE_STATE state_type;

      /// Apply event on a certain state 
      virtual void invoke(state_type& _state) const = 0;
    
      /// Returns an id string about this event
      virtual size_t id() const { return size_t(0); }
    };

    template<typename EVENT>
    using EventPtr = std::shared_ptr<EVENT>;

    template<typename EVENT>
    using EventList = std::vector<EVENT*>;
  }
}
