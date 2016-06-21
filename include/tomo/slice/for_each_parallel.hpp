#pragma once

#include <boost/thread.hpp>
#include "for_each.hpp"

namespace tomo
{
  namespace slice
  {
    namespace strategy
    {
      namespace for_each 
      {
        template<typename SLICEGRAPH>
        struct Parallel 
        {
          Parallel(SLICEGRAPH& _sliceGraph) :
            sliceGraph_(_sliceGraph) 
          {
          }

          template<typename SLICE>
          void insert(SLICE&& _slice)
          {
            sliceGraph_.insert(_slice);
          }
        private:
          SLICEGRAPH& sliceGraph_;
        };

        template<typename SLICEGRAPH>
        struct Parallel2Graphs
        {
          Parallel2Graphs(
              SLICEGRAPH& _a,
              SLICEGRAPH& _b) : 
          a_(_a),
          b_(_b) {}

          SLICEGRAPH& a_;
          SLICEGRAPH& b_;
        };

      }
    }

    namespace functor
    {
      namespace 
      {
        template<typename SLICE, typename FUNCTOR>
        struct Worker
        {
          typedef std::vector<SLICE const*> input_slices_type;
          typedef std::vector<SLICE> output_slices_type;

          Worker(input_slices_type const& _inputSlices,
              size_t _begin,
              size_t _end,
              output_slices_type& outputSlices_,
              FUNCTOR _functor) : 
            inputSlices_(_inputSlices),
            begin_(_begin),
            end_(_end),
            outputSlices_(outputSlices_),
            functor_(_functor)
          {
          }

          void operator()()
          {
            outputSlices_.reserve(end_-begin_);
            for (size_t i = begin_; i < end_; ++i)
            {
              auto&& _slice = inputSlices_[i];
              if (!_slice) continue;
              auto&& _newSlices = functor_(*_slice);
              for (auto& _newSlice : _newSlices)
                outputSlices_.emplace_back(_newSlice);
            }
          }

          std::vector<SLICE const*> const& inputSlices_;
          size_t begin_;
          size_t end_;
          std::vector<SLICE>& outputSlices_;
          FUNCTOR functor_;
        };
        
        template<typename SLICE, typename FUNCTOR>
        struct WorkerFor2
        {
          typedef std::vector<SLICE const*> input_slices_type;
          typedef std::vector<SLICE> output_slices_type;

          WorkerFor2(input_slices_type const& _inputSlices,
              size_t _begin,
              size_t _end,
              output_slices_type& _a,
              output_slices_type& _b,
              FUNCTOR _functor) : 
            inputSlices_(_inputSlices),
            begin_(_begin),
            end_(_end),
            a_(_a),
            b_(_b),
            functor_(_functor)
          {
          }

          void operator()()
          {
            a_.reserve(end_-begin_);
            b_.reserve(end_-begin_);
            for (size_t i = begin_; i < end_; ++i)
            {
              auto&& _slice = inputSlices_[i];
              if (!_slice) continue;
              functor_(*_slice,a_,b_);
            }
          }

          std::vector<SLICE const*> const& inputSlices_;
          size_t begin_;
          size_t end_;
          std::vector<SLICE>& a_;
          std::vector<SLICE>& b_;
          FUNCTOR functor_;
        };
      }

      template<typename SLICE>
      struct ForEach<Graph<SLICE>,strategy::for_each::Parallel<Graph<SLICE>>>
      {
        template<typename GRAPH, typename STRATEGY, typename FUNCTOR>
        void operator()(const GRAPH& _graph, STRATEGY _s, FUNCTOR f)
        {
          size_t _num = boost::thread::hardware_concurrency();
          if (_num < 2)
          {
            /// Use default non-parallel strategy if there is no multicore processor
            for_each(_graph,f);
            return;
          }

          boost::thread_group _threads;
          auto&& _slices = _graph.fetch();
          while (_slices.size() % _num)
          {
            _slices.push_back(nullptr);
          }

          std::vector<std::vector<SLICE>> _outSlices(_num);
          for (size_t i = 0; i < _num; ++i)
          {
            _threads.add_thread(new boost::thread(Worker<SLICE,FUNCTOR>(
                    _slices,
                    _slices.size()*i/_num,_slices.size()*(i+1)/_num,
                    _outSlices[i],f)));
          }
          TOMO_DBG(std::string("Have ") << _threads.size() << " threads.")

          _threads.join_all();
          for (auto& _slices : _outSlices)
          {
            for (auto& _slice : _slices)
            {
              _s.insert(_slice);
            }
            /// Free Memory
            std::vector<SLICE> _tmp;
            _slices.swap(_tmp);
          }
        }
      };

      template<typename SLICE>
      struct ForEach<Graph<SLICE>,strategy::for_each::Parallel2Graphs<Graph<SLICE>>>
      {
        template<typename GRAPH, typename STRATEGY, typename FUNCTOR>
        void operator()(const GRAPH& _graph, STRATEGY _s, FUNCTOR f)
        {
          size_t _num = boost::thread::hardware_concurrency();
          boost::thread_group _threads;
          auto&& _slices = _graph.fetch();
          while (_slices.size() % _num)
          {
            _slices.push_back(nullptr);
          }

          std::vector<std::vector<SLICE>> _aOut(_num);
          std::vector<std::vector<SLICE>> _bOut(_num);
          for (size_t i = 0; i < _num; ++i)
          {
            _threads.add_thread(new boost::thread(WorkerFor2<SLICE,FUNCTOR>(
                    _slices,
                    _slices.size()*i/_num,_slices.size()*(i+1)/_num,
                    _aOut[i],_bOut[i],f)));
          }
          TOMO_DBG(std::string("Have ") << _threads.size() << " threads.")

          _threads.join_all();
          for (auto& _slices : _aOut)
          {
            for (auto& _slice : _slices)
            {
              _s.a_.insert(_slice);
            }
            /// Free Memory
            std::vector<SLICE>().swap(_slices);
          }
          
          for (auto& _slices : _bOut)
          {
            for (auto& _slice : _slices)
            {
              _s.b_.insert(_slice);
            }
            /// Free Memory
            std::vector<SLICE>().swap(_slices);
          }
        }
      };

    }
  }
}

