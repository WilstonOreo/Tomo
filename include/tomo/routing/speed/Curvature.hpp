#pragma once 

#include <gex/algorithm/boost.hpp>
#include <gex/algorithm/for_each.hpp>
#include <gex/algorithm/step.hpp>
#include <gex/algorithm/perimeter.hpp>

#include <tomo/routing/average_speed.hpp>
#include "Optimize.hpp"

namespace tomo
{
  namespace routing
  {
    namespace strategy
    {
      struct Curvature : Optimize
      {
        Curvature(
            const Range& _speedRange, 
            Scalar _step = 5.0, 
            Scalar _curveLength = 0.5, 
            Scalar _gamma = 1.0) :
          Optimize(_speedRange,_step),
          curveLength_(_curveLength),
          gamma_(_gamma) {}

        template<typename THETA, typename SPEED>
        void adaptSpeed(const THETA& _theta, SPEED& _speed) const
        {
          float _speedFactor = pow(1.0 - std::min(_theta,THETA(180.0)) / 180.0,gamma());
          SPEED _quant = round(((_speed - speedRange().min()) * _speedFactor + speedRange().min()) / abs(step())) * step();
          _speed = std::min(std::max(_quant,SPEED(speedRange().min())),SPEED(speedRange().max()));
        }

        TBD_PROPERTY_REF(Scalar,curveLength)
        TBD_PROPERTY_REF(Scalar,gamma)
      };
    }

    namespace functor
    {
      template<typename PATH>
      struct SpeedOptimization<PATH,strategy::Curvature>
      {
        template<typename STRATEGY>
        void operator()(const PATH& _path, STRATEGY _strategy, PATH& _opt)
        {
          typedef typename PATH::point_type move_type;
          typedef std::pair<move_type,move_type> move_pair_type;
          typedef typename move_type::pos_type pos_type;
          
          if (_path.size() <= 2)
          {
            _opt = _path;
            return;
          }

          _opt.clear();
          auto& _length = _strategy.curveLength();

          gex::for_each<move_pair_type>(_path,[&](const move_type& _first, const move_type& _second)
          {
            pos_type _p0 = _first;
            pos_type _p1 = _second;
            _opt.push_back(_first);
            if (_first.extrusionWidth() == 0 || 
                _second.extrusionWidth() == 0) 
            {
              return;
            }

            auto _d = gex::distance(_p0,_p1);
            if (_d < _length*3.0)
            {
              _opt.emplace_back(0.5 * (_p0 + _p1),
                _first.speed(),
                _first.extrusionWidth());
              return;
            }
            pos_type _v = _length / _d * (_p1 - _p0);
            _opt.emplace_back(_p0 + _v,_first.speed(),_first.extrusionWidth());
            _opt.emplace_back(_p1 - _v,_second.speed(),_second.extrusionWidth());
          });
          _opt.push_back(_path.back());

          typedef gex::base::Curve<pos_type,false> curve_type;

          for (size_t i = 0; i < _opt.size(); ++i)
          {
            float _thetaSum = 0;
            float _leftSum = 0, _rightSum = 0;
            
            /// Left side
            for (size_t j = i; j > 1; --j)
            {
              auto&& _p0 = pos_type(_opt[j-1]);
              auto&& _p1 = pos_type(_opt[j]);
              auto&& _p2 = pos_type(_opt[j+1]);
              curve_type _curve(_p0,_p1,_p2);
 
              auto _theta = _curve.angle();
              _thetaSum += _theta * (1.0 - _leftSum / _length);

              _leftSum += gex::distance(_p0,_p1);
              if (_leftSum > _length) break;
            }

            /// Right side
            for (size_t j = i+1; j < _opt.size()-1; ++j)
            {
              auto&& _p0 = pos_type(_opt[j-1]);
              auto&& _p1 = pos_type(_opt[j]);
              auto&& _p2 = pos_type(_opt[j+1]);
              curve_type _curve(_p0,_p1,_p2);
              auto _theta = _curve.angle();
              _thetaSum += _theta * (1.0 - _rightSum / _length);
              _rightSum += gex::distance(_p0,_p1);
              if (_rightSum > _length) break;
            }
            auto&& _speed = _opt[i].speed();
            _strategy.adaptSpeed(_thetaSum,_speed);
            _opt[i].speed(_speed);
          }

          auto _averageSpeed = tomo::routing::average_speed(_opt);
          if (_averageSpeed != _averageSpeed)
          {
            _averageSpeed = _strategy.speedRange().min();
          }

          float _sum = 0.0;
          for (size_t i = 1; i < _opt.size(); ++i)
          {
            float _factor = _sum / _length;
            auto&& _p0 = pos_type(_opt[i-1]);
            auto&& _p1 = pos_type(_opt[i]);
            auto&& _speed = _opt[i].speed();
            _speed = _strategy.quantize(_averageSpeed * (1.0 - _factor) + _speed * _factor);
            if (_speed <= _averageSpeed) break;
            _sum += gex::distance(_p0,_p1);
            if (_sum > _length) break;
          }

          _sum = 0.0;
          for (size_t i = _opt.size()-1; i > 1; --i)
          {
            float _factor = _sum / _length;
            _opt[i].speed() = _strategy.quantize(_averageSpeed * (1.0 - _factor) + _opt[i].speed() * _factor);
            if (_opt[i].speed() <= _averageSpeed) break;
            _sum += gex::distance(_opt[i],_opt[i-1]);
            if (_sum > _length) break;
          }
          _opt.front().speed() = _strategy.quantize(_averageSpeed);
          _opt.back().speed() = _strategy.quantize(_averageSpeed);

          for (auto& _move : _opt)
          {
            if (float(_move.speed()) != float(_move.speed()))
              _move.speed() = _averageSpeed;
          }
        }
      };
    }
  }
}



