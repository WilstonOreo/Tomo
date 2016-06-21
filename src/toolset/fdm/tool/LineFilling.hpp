#pragma once

#include "tomo/geometry/algorithm/BoundingSegments.hpp"

namespace tomo
{
  namespace fdm
  {
    struct LineFillingPattern
    {
      typedef typename SliceGraph::slice_type::Polygon::model_type model_type;
      typedef typename geometry::prim::MultiLineString<model_type> MultiLineString;
      typedef typename geometry::prim::LineString<model_type> LineString;
      typedef typename geometry::prim::Segment<model_type> Segment;

      template<typename BOUNDS>
      void operator()(const BOUNDS& _bounds, float _gap, float _angle)
      {
        pattern_.clear();

        auto _boundingSegments = geometry::algorithm::BoundingSegments<Segment>()(_bounds,_angle);
        auto _fillingDirection = _boundingSegments.second[0] - _boundingSegments.first[0];

        unsigned _nLines = _fillingDirection.length() / _gap;
        auto _inc = _fillingDirection * (1.0 / _nLines);

        pattern_.reserve(_nLines*2);
        Segment _seg(_boundingSegments.first[0] + 0.5*_inc,_boundingSegments.first[1] + 0.5*_inc);

        for (unsigned i = 1; i < _nLines; ++i)
        {
          pattern_.push_back(_seg[0]);
          pattern_.push_back(_seg[1]);
          
          _seg[0] += _inc;
          _seg[1] += _inc;
          std::swap(_seg[0],_seg[1]);
        }
      }

      TBD_PROPERTY_REF_RO(LineString,pattern)
    };

    struct LineFillingOperation
    {
      typedef typename Slice::MultiPolygon MultiPolygon;
      typedef typename geometry::prim::MultiLineString
      <typename MultiPolygon::model_type> MultiLineString;

      template<typename SOURCES, typename CONFIGURATOR>
      MultiLineString operator()(const SOURCES& _sources,
                                 const Slice& _slice,
                                 const CONFIGURATOR& _conf,
                                 MultiPolygon& _out)
      {
        MultiPolygon&& _in = _slice.fetchPolygons(_sources);

        if (_in.empty()) return MultiLineString();

        /// Filling returns no polygon
        _out.clear();

        MultiLineString _filling;
        boost::geometry::intersection(_in,_conf.pattern().pattern(),_filling);

        return _filling;
      }
    };

    struct LineFillingConfigurator : Configurator
    {
      LineFillingConfigurator() :
        Configurator("LineFilling"),
        speed_(50),
        extruderSpeed_(1),
        gap_(3),
        angle_(45)
      {}

      void operator()(tbd::Config* _config)
      {
        speed(_config);
        extruderSpeed(_config);
        gap(0.5);
        angle(45.0);
      }

      void pattern(const LineFillingPattern& _pattern) { pattern_ = _pattern; }
      const LineFillingPattern& pattern() const { return pattern_; };

      TBD_PROPERTY_MODIFY_CFG(float,speed,50)
      TBD_PROPERTY_MODIFY_CFG(float,extruderSpeed,1)
      TBD_PROPERTY_MODIFY_CFG(float,gap,3)
      TBD_PROPERTY_MODIFY_CFG(float,angle,45)
    private:
      LineFillingPattern pattern_;
    }; 

    struct LineFilling : SlicingTool<LineFillingConfigurator,LineFillingOperation>, tool::ToolTraits
    {
      LineFilling(const LineFillingPattern& _pattern,
                  const tag_type _tag, 
                  const sources_type& _sources) :
        SlicingTool(_tag,_sources)
       {
         LineFillingConfigurator::pattern(_pattern);
       }
    };
  }
}
