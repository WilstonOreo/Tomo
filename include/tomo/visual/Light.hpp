#pragma once

#include <gex/base.hpp>
#include <vector>

namespace tomo
{
  namespace visual
  {
    template<class TRACKER, class COLOR> struct Light : TRACKER
    {
      /// tracker type
      typedef TRACKER Tracker;
      typedef COLOR Color;
      /// coordinate type from tracker
      typedef typename Tracker::Point Point;
      typedef typename Tracker::Point::Scalar Scalar;
      /// point type from tracker
      typedef gex::base::Point<gex::base::Model<4,Scalar>> Point4;

      Light() {}
      Light( const Tracker& _tracker,
             const Color& _ambient,
             const Color& _diffuse,
             const Color& _specular,
             const Scalar _intensity,
             Scalar _shadows,
             Scalar _radius,
             bool _positional=true) :
        Tracker(_tracker),
        ambient_(_ambient),
        diffuse_(_diffuse),
        specular_(_specular),
        intensity_(_intensity),
        shadows_(_shadows),
        radius_(_radius),
        positional_(_positional)
      {
      }
      Point4 eye4() const
      {
        Point e=Tracker::eye();
        return Point4(e.x(),e.y(),e.z(),positional_?1.0:0.0);
      }

      TBD_PROPERTY_REF(Color,ambient)
      TBD_PROPERTY_REF(Color,diffuse)
      TBD_PROPERTY_REF(Color,specular)
      TBD_PROPERTY(Scalar,intensity)
      TBD_PROPERTY(Scalar,shadows)
      TBD_PROPERTY(Scalar,radius)
      TBD_PROPERTY(bool,positional)
    };
  }
}
