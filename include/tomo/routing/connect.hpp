#pragma once

#include <tomo/slice/centroid.hpp>

#include "for_each_path.hpp"
#include <gex/algorithm/join.hpp>

namespace tomo
{
  namespace routing
  {
    namespace strategy
    {
      namespace connect
      {
        template<typename JUNCTION>
        struct NoOpt
        {
          typedef JUNCTION junction_type;

          NoOpt(const junction_type& _junction) : 
            junction_(_junction) {}
 
          junction_type const& junction() const 
          {
            return junction_;
          }

        private:
          junction_type const& junction_;
        };

        template<typename JUNCTION>
        struct NearestInsertion : NoOpt<JUNCTION>
        {
          typedef JUNCTION junction_type;

          NearestInsertion(const junction_type& _junction) :
            NoOpt<junction_type>(_junction) {}          
        };
      }
    }

    namespace functor
    {
      /// Connect all paths to a single out with a specific strategy
      template<typename PATHS, typename STRATEGY>
      struct Connect {};
    }

    using functor::Connect;

    template<typename PATHS, typename STRATEGY, typename OUTPUT>
    void connect(const PATHS& _a, STRATEGY _s, OUTPUT& _out)
    {
      Connect<PATHS,STRATEGY>()(_a,_s,_out);
    };

    template<typename PATHS, typename STRATEGY>
    typename Connect<PATHS,STRATEGY>::output_type connect(const PATHS& _paths, STRATEGY _s)
    {
      typename Connect<PATHS,STRATEGY>::output_type _out;
      connect(_paths,_s,_out);
      return _out;
    }

    namespace functor
    {
      template<typename SLICE, typename JUNCTION>
      struct Connect<SLICE,strategy::connect::NoOpt<JUNCTION>>
      {
        typedef typename SLICE::path_type path_type;
        typedef path_type output_type;

        template<typename A, typename B, typename STRATEGY>
        void operator()(const A& _slice, STRATEGY _s, B& _path)
        {
          for_each_path(_slice,[&](const path_type& _p)
          {
            gex::connect(_path,_p,_s.junction());
          });
        }
      };
 
      template<typename SLICE, typename JUNCTION>
      struct Connect<SLICE,strategy::connect::NearestInsertion<JUNCTION>>
      {
        typedef typename SLICE::path_type path_type;
        typedef path_type output_type;

        template<typename A, typename B, typename STRATEGY>
        void operator()(const A& _slice, STRATEGY _s, B& _path)
        {
          std::vector<path_type const*> _connections;
          _connections.reserve(_slice.paths().size());
          gex::algorithm::functor::detail::connection_list(
              tomo::slice::centroid(_slice),_slice.paths(),_connections);

          for (auto& _connection : _connections)
          {
            gex::connect(_path,*_connection,_s.junction());
          }
        }
      };
    }
  }
}

