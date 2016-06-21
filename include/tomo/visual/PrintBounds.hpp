#pragma once
#include <gex/base.hpp>

namespace tomo
{
  namespace scene
  {
    struct PrintBounds : gex::Bounds3
    {
      typedef gex::Vec3 Vec;
      typedef gex::Point3 Point;

      PrintBounds() {}
      PrintBounds( const Vec& _size ) :
        Bounds( Point( -_size.x()/2, -_size.y()/2, 0 ),
                Point( _size.x()/2, _size.y()/2, _size.z() ) )
      {
      }
    };
  }
}

