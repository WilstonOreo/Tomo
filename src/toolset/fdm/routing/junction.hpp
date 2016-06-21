#pragma once

namespace tomo
{
  namespace fdm
  {
    namespace routing
    {
      namespace junction
      {
        struct Straight 
        {
          Straight(float _speed = 0.0) :
            speed_(_speed) {}
          
          template<typename A, typename B, typename C>
          void operator()(const A& _front, const B& _back, C& _out)
          {
            if (!_out.empty())
            {
              if (gex::distance(_front.back(),_out.back()) < 0.001)
              {
                _out.back().speed() = speed();
                _out.back().setTravel();
              } else
              {
                _out.emplace_back(_front.back(),speed(),0.0); 
              }
            } else
            {
              _out.emplace_back(_front.back(),speed(),0.0); 
            }
            if (gex::distance(_front.back(),_back.front()) <= 0.001)
            {
              _out.emplace_back(_front.back());
              return;
            }           
            _out.push_back(_back.front());
          }

          TBD_PROPERTY_REF(float,speed)
        };

        struct ZAxis : Straight
        {
          ZAxis(float _speed, float _zSpeed) :
            Straight(_speed),
            zSpeed_(_zSpeed) {}

          template<typename A, typename B, typename C>
          void operator()(const A& _front, const B& _back, C& _out)
          {
            if (!_out.empty())
            {
              if (gex::distance(_front.back(),_out.back()) < 0.001)
              {
                _out.back().speed() = speed();
                _out.back().setTravel();
              } else
              {
                _out.emplace_back(_front.back(),zSpeed(),0.0); 
              }
            } else
            {
              _out.emplace_back(_front.back(),speed(),0.0); 
            }
            if (gex::distance(_front.back(),_back.front()) <= 0.001)
            {
              _out.emplace_back(_front.back());
              return;
            }          

            gex::Point3 _p(_front.back());
            _p(gex::Z) = _back.front()(gex::Z);
            _out.emplace_back(_p,speed(),0.0); 
            if (gex::distance(_p,_back.front()) <= 0.00001)
            {
              return;
            }

            _out.push_back(_back.front());  
          }

          TBD_PROPERTY_REF(float,zSpeed)
        };
      }
    }
  }
}
