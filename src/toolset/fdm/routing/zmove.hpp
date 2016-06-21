#pragma once

namespace tomo
{
  namespace fdm
  {
    namespace routing
    {
      namespace detail
      {
        struct Candidate
        {
          typedef gex::Point3 point_type;
          Candidate() {}
          Candidate(float _distance, const point_type& _point) :
          distance_(_distance),
          point_(_point) {}

        friend bool operator<(const Candidate& _lhs, const Candidate& _rhs)
        {
          return _lhs.distance() < _rhs.distance();
        }

        template<typename CANDIDATES>
        Candidate minimum(const CANDIDATES& _candidates)
        {
          Candidate _candidate;
          std::pair<Candidate*,gex::Scalar> _closest(nullptr,gex::inf());
          for (auto& _candidate : _candidates)
          {
            auto&& _distance = gex::sqrDistance(_candidate.point(),point());
            if (_distance < _closest.second)
              _closest = { &_candidate,_distance };
          }
          if (!_closest.first)
            return *this;
          return *_closest.first;
        }

        TBD_PROPERTY_REF(float,distance)
        TBD_PROPERTY_REF(point_type,point)
      };

      template<size_t CANDIDATES_PER_LAYER>
      struct CandidatesPerLayer
      {
        typedef PathLayer* pathlayer_ptr_type;
        typedef std::array<Candidate,CANDIDATES_PER_LAYER> array_type;

        CandidatePerLayer()
        {
          clear();
        }

        Candidates(pathlayer_ptr_type _path) :
        {
          generate(_path);
        }

        void clear()
        {
          numCandidates_ = 0;
        }

        bool push_back(const Candidate& _candidate)
        {
          if (numCandidates_ >= CANDIDATES_PER_LAYER) return false;
          candidates_[numCandidates_] = _candidate;
          ++numCandidates_;
          return true;
        }

        void push_back(std::vector<Candidate>&& _candidates)
        {
          std::sort(_candidates.begin(),_candidates.end());
          for (auto& _candidate : _candidates)
          {
            if (!push_back(_candidate)) break;
          }
        }

        TBD_PROPERTY_REF(pathlayer_ptr_type,pathLayer)
        TBD_PROPERTY_REF(size_t,numCandidates)
        TBD_PROPERTY_REF(array_type,candidates)
      };

      /*
        typedef std::array<Candidate,CANDIDATES_PER_LAYER> candidates_type;
      template<size_t CANDIDATES_PER_LAYER>
        candidates_type zMovementCandidates(const candidates_type& _lowerCandidates)
        {
          std::vector<Candidate> _candidates;
          constexpr size_t _numCandidates = 8;

          if (_lowerCandidates.empty())
          {
for (auto& _path : _isle.paths())
            {

            }
          }
          else
          {
for (auto& _candidate : _lowerCandidates)
            {
for (auto& _path : _isle.paths())
              {
                auto&& _closestPoint = gex::closestPoint(_path.ring(),_candidate.point());
                _distance = gex::sqrDistance(_closestPoint,_candidate.point());
                _candidates.emplace_back(_distance,_move);
              }
            }
          }

          std::sort(_candidates.begin(),_candidates.end());
          return _bestCandidates;
        }
      */
      }

      struct ZPath
      {
        typedef gex::Point3 point_type;
        typedef std::pair<pathlayer_ptr_type,candidates_type> pair_type;

        template<typename PATHLAYERS>
        void generate(PATHLAYERS& _layers)
        {
          Candidate _candidate;
for (auto& _layer : _layers)
          {
            auto _candidates = zMovementCandidates();
            auto&& _candidate = _candidate.minimum(_candidates);
            points_.push_back(_candidate.point());
          }

          std::sort(zMoves_.begin(),zMoves.end(),[&](const pair_type& _lhs, const pair_type& _rhs) -> bool
          {
            return _lhs.point().z() < _rhs.point().z();
          });
        }

        TBD_PROPERTY_REF(std::vector<pair_type>,zMoves)
      };
    }
  }
}
