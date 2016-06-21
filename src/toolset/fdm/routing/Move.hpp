#pragma once

#include <tomo/base/gex.hpp>
#include <tomo/base/types/Speed.hpp>

namespace tomo
{
  namespace fdm
  {
    /// A move defines the point where the tool can move to
    struct Move 
    {
      typedef typename gex::Point3 pos_type;
      typedef tomo::base::Speed speed_type;
      typedef tomo::Scalar Scalar;

      static constexpr size_t SizeAtCompileTime = 3;

      Move() : 
        extrusionWidth_(0),
        speed_(0) {} 
      Move(
        const pos_type& _pos, 
        const speed_type& _speed = 60.0,
        const Scalar& _extrusionWidth = 0.0) :
        pos_(_pos)
      {
        speed(_speed);
        extrusionWidth(_extrusionWidth);
      } 

      inline bool travel() const { return (!extrusionWidth_) || retract(); } 
      
      inline bool retract() const
      {
        return extrusionWidth_ == retractValue();
      }

      inline void setTravel(bool _retract = false)
      {
        extrusionWidth_ = _retract ? retractValue() : 0;
      }


      inline void extrusionWidth(const Scalar& _w)
      {
        if (_w <= 0.0)
        {
          setTravel();
          return;
        }
        if (_w >= maxExtrusionWidth())
        {
          extrusionWidth_ = retractValue() - 1;
          return;  
        } 
        extrusionWidth_ = _w / maxExtrusionWidth() * std::numeric_limits<uint16_t>::max();
      }
      
      inline Scalar extrusionWidth() const
      {
        if (retract() || travel()) return 0.0;
        return extrusionWidth_ * extrusionWidthNormalizer();
      }

      inline speed_type speed() const
      {
        return speed_type(speed_ / speedFactor());
      }

      Scalar const& operator()(Axis const& _axis) const
      {
        return pos_(size_t(_axis));
      }
      
      Scalar& operator()(Axis const& _axis)
      {
        return pos_(size_t(_axis));
      }
      
      Scalar const& operator()(size_t _axis) const
      {
        return pos_(_axis);
      }
      
      Scalar& operator()(size_t _axis)
      {
        return pos_(_axis);
      }

      inline void speed(const speed_type& _speed)
      {
        if (_speed > maxSpeed())
        {
          speed_ = maxSpeed() * speedFactor();
          return;
        }
        if (_speed < 0.0) 
        {
          speed_ = 0.0;
          return;
        }
        speed_ = float(_speed) * speedFactor();
      }

      operator pos_type&()
      {
        return pos_;
      }
     
      operator pos_type() const
      {
        return pos_;
      }

      Scalar& x() { return pos_.x(); }
      Scalar& y() { return pos_.y(); }
      Scalar& z() { return pos_.z(); }
      Scalar const& x() const { return pos_.x(); }
      Scalar const& y() const { return pos_.y(); }
      Scalar const& z() const { return pos_.z(); }
  
      TBD_PROPERTY_REF(pos_type,pos)
    private:
      inline static Scalar speedFactor()
      {
        return 256.0;
      }

      inline static Scalar maxSpeed() 
      {
        return std::numeric_limits<uint16_t>::max() * speedFactor();
      }


      inline static Scalar maxExtrusionWidth()
      {
        return 2.0;
      }

      inline static float extrusionWidthNormalizer()
      {
        return maxExtrusionWidth() / (std::numeric_limits<uint16_t>::max());
      }
      
      static constexpr uint16_t retractValue()
      {
        return 0xFFFF;
      }

      uint16_t extrusionWidth_, speed_;
    };
  }
}

#include <gex/algorithm/distance.hpp>

/// Distance between move 
namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<>
      struct SqrDistance<tomo::fdm::Move,tomo::fdm::Move,Scalar>
      {
        typedef tomo::fdm::Move move_type;
        typedef move_type::pos_type pos_type;

        inline Scalar operator()(const move_type& _prev, const move_type& _next)
        {
          return sqrDistance(pos_type(_prev),pos_type(_next));
        }
      };
    }
  }
}

GEX_REGISTER_POINT(tomo::fdm::Move,3,gex::Scalar)
