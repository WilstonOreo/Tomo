#pragma once
#include <tbd/parameter.h>
#include <tbd/classregister.h>
#include <gex/algorithm/pattern.hpp>
#include <gex/algorithm/fill.hpp>
#include "../routing/slice.hpp"

namespace tomo
{
  namespace fdm
  {
    namespace pattern
    {
      struct Interface 
      {
        TOMO_BASE_TYPES        
        typedef gex::MultiLineString output_type;
        typedef Slice::polygon_type polygon_type;
        typedef polygon_type::bounds_type bounds_type;
 
        /// Pattern generation algorithm needs to be defined in derived classes
        virtual output_type generate(
            const bounds_type& _bounds, 
            const polygon_type& _polygon) const = 0;
      };

      class Linear : public Interface
      {
      public:
        Linear(Scalar _angle, Scalar _density, bool _connect) : 
          angle_(_angle),
          density_(_density),
          connect_(_connect) {}
        
        Linear() : 
          angle_(0.0),
          density_(1.0),
          connect_(true) {}

        inline output_type generate(
            const bounds_type& _bounds, 
            const polygon_type& _polygon) const
        {
          typedef gex::algorithm::pattern::Linear<gex::Point2> LinearPattern;
          gex::for_each<gex::Ring>(_polygon,[&](const gex::Ring& _ring)
          {
            //TOMO_DBG(_ring.size() << " " << boost::geometry::area(_ring))
          });
          
          auto&& _lineStrings = gex::fillBounds(_polygon,_bounds,LinearPattern(angle(),density()));
          size_t _s = _lineStrings.size();
          if (_s < 2 || !connect_) return _lineStrings;
    
          output_type _output;
          gex::LineString _lineString = _lineStrings.front();
          for (size_t i = 0; i < _lineStrings.size()-1; ++i)
          {
            gex::LineString _connection;
            gex::Point2 _p0 = _lineStrings[i].back() + (_lineStrings[i].front() - _lineStrings[i].back())*0.99; 
            gex::Point2 _p1 = _lineStrings[i+1].front() + (_lineStrings[i+1].back() - _lineStrings[i+1].front())*0.99; 
            _connection.push_back(_p0);
            _connection.push_back(_p1);

            if (!gex::intersects(_connection,_polygon))
            {
              gex::LineString _tmp;
              gex::connect(_lineString,_lineStrings[i+1],_tmp,
                  gex::algorithm::junction::Straight<gex::LineString,gex::LineString>());
              _lineString = _tmp;
            } else
            {
              _output.push_back(_lineString);
              _lineString.clear();
              _lineString = _lineStrings[i+1];
            }
          }
          _output.push_back(_lineString);

          return _output;
        }

        TOMO_TYPEID_DECL("linear")
          
        TBD_PROPERTY_REF(Scalar,angle)
        TBD_PROPERTY_REF(Scalar,density)
        TBD_PROPERTY_REF(bool,connect)
      };

      class Hexagon : public Interface
      {
      public:
        Hexagon(Scalar _angle, 
            Scalar _density,
            Scalar _dist) : 
          angle_(_angle),
          density_(_density),
          distance_(_dist) {}
        
        Hexagon() : 
          angle_(0.0),
          density_(1.0),
          distance_(0.2) {}
        
        float getAngle() const { return angle_; } 
        void setAngle(float _angle) { angle_ = _angle; }

        inline output_type generate(
            const bounds_type& _bounds, 
            const polygon_type& _polygon) const
        {
          typedef gex::algorithm::pattern::Hexagon<gex::Point2> HexagonPattern;
          return gex::fillBounds(_polygon,_bounds,HexagonPattern(angle(),density(),distance()));
        }

        TOMO_TYPEID_DECL("hexagon")
        
        TBD_PROPERTY_REF(Scalar,angle)
        TBD_PROPERTY_REF(Scalar,density)
        TBD_PROPERTY_REF(Scalar,distance)
      };
 
      class Concentric : public Interface  
      {
      public:
        Concentric(Scalar _distance, int _number = 0) : 
          distance_(_distance) {}
        Concentric() : 
          distance_(0.5) {}

        inline output_type generate(
            const bounds_type& _bounds, 
            const polygon_type& _polygon) const
        {
          typedef gex::algorithm::pattern::Concentric<polygon_type,gex::Point2> ConcentricPattern;
          return gex::fillBounds(_polygon,_bounds,ConcentricPattern(_polygon,distance()));
        }

        TOMO_TYPEID_DECL("concentric")
        TBD_PROPERTY_REF(Scalar,distance)
        TBD_PROPERTY_REF(int,number)
      };
    }
  }
}
