#pragma once

#include <list>
#include <gex/polygon.hpp>
#include "Generic.hpp"
#include "Set.hpp"

namespace tomo
{
  namespace slice
  {
    template<gex::Axis AXIS = gex::Z>
    struct Orthogonal : 
      public Generic<gex::Polygon>
    {
      typedef Generic<gex::Polygon> inherited_type;
      typedef typename inherited_type::tag_type tag_type;
      typedef typename inherited_type::polygon_type polygon_type;
      typedef typename gex::Range range_type;

      typedef Orthogonal<AXIS> type;
      typedef gex::Bounds3 bounds_type;
      typedef SetConst<type> sliceset_type;

      Orthogonal(const tag_type& _tag = invalidTag()) : 
        inherited_type(_tag),
        correct_(false) {}

      Orthogonal(
          const range_type& _range,
          const tag_type& _tag = invalidTag()) : 
        inherited_type(_tag),
        range_(_range) {}

      Orthogonal(
          const range_type& _range,
          const polygon_type& _polygon,
          const tag_type& _tag = invalidTag()) :  
        inherited_type(_polygon,_tag),
        correct_(false),
        range_(_range) {}

      Orthogonal(const Orthogonal& _rhs) : 
        inherited_type(_rhs.polygon()), 
        correct_(false),
        range_(_rhs.range_) {}
/*
      void swap(Orthogonal& _rhs)
      {
        std::swap(range_,_rhs.range_);
        std::swap(polygon(),_rhs.polygon());
      }

      Orthogonal& operator=(Orthogonal _rhs)
      {
        if (this != &_rhs) swap(_rhs);
        return *this;
      }
*/
      static constexpr gex::Axis axis() { return AXIS; } 

      bounds_type bounds() const { return inherited_type::polygon().bounds().expand(axis(),range()); }

      void update()
      {
        polygon().update();
        correct_ = true;
      }

      polygon_type& polygon() 
      {
        correct_ = false;
        return inherited_type::polygon();
      }

      const polygon_type& polygon() const 
      {
        return inherited_type::polygon();
      }

      range_type& range() 
      {
        correct_ = false;
        return range_;
      }
      
      gex::Scalar height() const
      {
        return range().length();
      }

      TBD_PROPERTY_RO(bool,correct)
      TBD_PROPERTY_REF_RO(range_type,range)
    };
  }
}
