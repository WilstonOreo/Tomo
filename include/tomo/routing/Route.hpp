#pragma once

#include "Event.hpp"
#include <map>
#include <memory>
#include <tbd/seq_op.h>
#include <tbd/property.h>

namespace tomo
{
  namespace routing
  {
    template<typename PATH, typename MACHINE_STATE>
    struct Route
    {
      typedef PATH path_type;
      typedef typename PATH::point_type move_type;
      typedef MACHINE_STATE state_type;
      typedef Event<state_type> event_type;
      typedef EventPtr<event_type> event_ptr_type;
      typedef EventList<event_type> eventlist_type;
      typedef size_t eventid_type;
      typedef std::map<eventid_type,std::vector<event_ptr_type>> eventmap_type;

      Route(const state_type& _initial = state_type()) : 
        initial_(_initial)
      {}

      void operator += (const path_type& _path)
      {
        path_.push_back( _path );
      }

      /// Insert a new event at a certain position
      template<typename EVENT, typename...ARGS>
      Route& eventAtPos(eventid_type _id, ARGS&&..._args)
      {
        newEventList(_id);
        events_.at(_id).emplace_back(new EVENT(_args...));
        return *this;
      }

      Route& addEvents(eventid_type _id, const eventlist_type& _events)
      {
        newEventList(_id);
        auto& _eventList = events_.at(_id);
        for (auto& _event : _events)
          _eventList.push_back(event_ptr_type(_event));
        return *this;
      }
      
      Route& addEvents(const eventlist_type& _events)
      {
        return addEvents(size(),_events);
      }


      template<typename EVENT, typename...ARGS>
      Route& event(ARGS&&..._args)
      {
        return eventAtPos<EVENT>(size(),_args...);
      }

      void clear()
      {
        path_.clear();
        events_.clear();
      }
     
      bool empty() const
      {
        return path_.empty();
      }

      size_t size() const
      {
        return path_.size();
      }

      path_type& path() 
      {
        return path_;
      }

      std::vector<event_type*> events(eventid_type _id)
      {
        std::vector<event_type*> _result;
        if (!events_.count(_id)) return _result;

        auto& _events = events_[_id];
        for (auto& _event : _events)
          _result.push_back(_event.get());
        return _result;
      }
      
      std::vector<event_type const*> events(eventid_type _id) const
      {
        std::vector<event_type const*> _result;
        if (!events_.count(_id)) return _result;
        auto& _events = events_.at(_id);
        for (auto& _event : _events)
          _result.push_back(_event.get());
        return _result;
      }

      TBD_PROPERTY_REF(state_type,initial)
      TBD_PROPERTY_REF_RO(path_type,path)
      TBD_PROPERTY_REF(eventmap_type,events)
    
    private:
      void newEventList(eventid_type _id)
      {
        if (events_.count(_id) == 0)
        {
          events_.insert(std::make_pair(_id,std::vector<event_ptr_type>()));
        }
      }
    };
  }
}

