#pragma once
#include <algorithm>
#include <gex/base.hpp>

namespace tomo
{
  namespace visual
  {
    template<
    class COORD,
          bool XNEG=false,
          bool YNEG=false,
          bool ZNEG=false
          > struct Tracker
    {
      typedef COORD Coord;
      /// point type
      typedef gex::base::Model<3,COORD> model_type;
      typedef gex::base::Point<model_type> Point;
      /// vector type
      typedef gex::base::Vec<model_type> Vec;
      typedef gex::base::PolarVec<Coord> PolarVec;
      static const bool xNeg_ = XNEG;
      static const bool yNeg_ = YNEG;
      static const bool zNeg_ = ZNEG;

      Tracker() : 
        center_(0.0,0.0,0.0),
        direction_(0.0,0.0,1.0)
      {
      }

      Tracker( const Tracker& _tracker )
      {
        center_ = _tracker.center_;
        direction_ = _tracker.direction_;
      }
      Tracker( const Point& _center, const PolarVec& _direction ) :
        center_(_center),
        direction_(_direction)
      {
      }
      void track( int x, int y, int z )
      {
        direction_ += PolarVec(
                        xNeg_?-x:x,
                        yNeg_?-y:y,
                        zNeg_?-z:z);
      }
      /// get tracker position
      Point eye() const
      {
        return center_ + Vec(direction_);
      }
      void eye( const Point& _pos )
      {
        direction_ = _pos - center_;
      }
      /// target position
      TBD_PROPERTY_REF(Point,center);
      /// tracker's postion relatively to target
      TBD_PROPERTY_REF(PolarVec,direction);
    };
  }
}
