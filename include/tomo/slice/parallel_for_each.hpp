#pragma once

#include <tbb/parallel_for_each.h>
#include <tbb/task_scheduler_init.h> 
#include "Graph.hpp"

namespace tomo
{
  namespace slice
  {
    template<typename SLICES, typename STRATEGY, typename FUNCTOR>
    void parallel_for_each(const SLICES&, SLICES&, STRATEGY, FUNCTOR);

    namespace strategy
    {
      template<typename SLICE>
      struct SlicesOnly
      {
      };
    }

    namespace functor
    {
      template<typename STRATEGY>
      struct Task {};

      template<typename SLICE>
      struct Task<strategy::SlicesOnly<SLICE>>
      {
        Task(std::vector<SLICE*> const& _slices, 
            size_t _begin, size_t _end) : 
          slices_(_slices),
          begin_(_begin),
          end_(_end) {}

        template<typename FUNCTOR>
        void operator()(FUNCTOR _f)
        {
          for (size_t i = begin_; i < end_; ++i)
          {
            _f(*slices_[i],output_);
          }
        };

        TBD_PROPERTY_REF_RO(std::vector<SLICE>,output)
      private:
        std::vector<SLICE const*> const& slices_;
        size_t begin_, end_;
      };

      template<typename STRATEGY>
      struct TaskMerge {};

      template<typename SLICE>
      struct TaskMerge<strategy::SlicesOnly<SLICE>>
      {
        template<typename TASKS, typename OUTPUT>
        void operator()(const TASKS& _tasks, OUTPUT& _out)
        {
          for (auto& _task : _tasks)
          {
            for (auto& _slice : _task->sliceOutput())
              _out.insert(_slice);
          }
        }
      };
      
      template<typename SLICES, typename TASK>
      struct ParallelForEach {};

      template<typename SLICE, typename STRATEGY>
      struct ParallelForEach<Graph<SLICE>,Task<STRATEGY>>
      {
        template<typename SLICES, typename FUNCTOR>
        void operator()(const SLICES& _in, SLICES& _out, STRATEGY _strategy, FUNCTOR _f)
        {
          tbb::task_scheduler_init init;  // Automatic number of threads
          auto&& _slices = _in.fetchSlices();

          typedef Task<STRATEGY> task_type;
          std::vector<std::shared_ptr<task_type>> _tasks;
          size_t _begin = 0;
          size_t _end = 0;
          size_t _inc = std::max(size_t(1),size_t(_slices.size() / 64));
          while (1)
          {
            _end = _begin + _inc;
            if (_end >= _slices.size()) break;
            _tasks.emplace_back(new task_type(_slices,_begin,_end));
            _begin = _end;
          }
          _tasks.emplace_back(new task_type(_slices,_begin,_slices.size()));

          tbb::parallel_for_each( _tasks.begin(),_tasks.end(),[&](std::shared_ptr<task_type>& _task)
          {
            (*_task)(_f);
          });

          TaskMerge<STRATEGY>()(_tasks,_out);
        }
      };
    }

    using functor::ParallelForEach;

    template<typename SLICES, typename STRATEGY, typename FUNCTOR>
    void parallel_for_each(const SLICES& _slices, SLICES& _out, STRATEGY _strategy, FUNCTOR _f)
    {
      ParallelForEach<SLICES,functor::Task<STRATEGY>>()(_slices,_out,_strategy,_f);
    }
  }
}
