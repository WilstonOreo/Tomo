#pragma once

#include "tomo/geometry/prim/Polygon.hpp"
#include "tomo/tool/Output.hpp"
#include "tomo/tool/Tool.hpp"
#include "SliceTag.hpp"

namespace tomo
{
  namespace slice
  {
    template<typename OUTPUT>
    struct Slice : geometry::prim::Primitive3<typename OUTPUT::scalar_type>
    {
      typedef OUTPUT output_type;
      typedef typename output_type::tag_type tooltag_type;
      typedef typename output_type::sources_type toolsources_type;
      typedef typename output_type::state_type state_type;
      typedef typename output_type::MultiPolygon MultiPolygon;

      typedef std::map<tooltag_type,output_type> tooloutput_map_type;

      typedef std::vector<Slice const*> link_cont_type;

      TOMO_PRIMITIVE(geometry::base::Model3<typename output_type::scalar_type>);

      typedef geometry::prim::Polygon2<scalar_type> Polygon;

      /// Creates a dummy slice only
      Slice(range_type _zRange) : zRange_(_zRange) {}

      Slice(range_type _zRange, const Polygon& _polygon, const SliceTag& _tag) :
        zRange_(_zRange), polygon_(_polygon), tag_(_tag)
      {
      }

      /// Clear tool output
      void clear()
      {
        toolOutputs_.clear();
      }

      /// Inserts a certain output from a tool
      void insert(tooltag_type _tag, const output_type& _toolOutput)
      {
        toolOutputs_.insert(std::make_pair(_tag,_toolOutput));
      }

      /// Retrieves a tool output by tag
      output_type const* get(tooltag_type _tag) const
      {
        if (toolOutputs_.count(_tag) == 0) return nullptr;
        return &(toolOutputs_.at(_tag));
      }

      MultiPolygon fetchPolygons(const toolsources_type& _sources) const
      {
        MultiPolygon _out;
        
        for (const auto& _src : _sources)
        {
          if (_src == 0) 
          {
            _out.push_back(polygon());
            continue;
          }

          auto _toolOutput = toolOutputs_.find(_src);
          if (_toolOutput == toolOutputs_.end()) continue;

          _out.insert(_out.end(),_toolOutput->second.polygons().begin(),
                                 _toolOutput->second.polygons().end());
        }
        _out.update();

        return _out;
      }


      const bounds_type bounds() const
      {
        const typename Polygon::bounds_type& _pB = polygon_.bounds();

        return bounds_type(
                 point_type(_pB.min().x(),_pB.min().y(),zRange_.min()),
                 point_type(_pB.max().y(),_pB.max().y(),zRange_.max()));
      }

      friend bool operator<(const Slice& _a, const Slice& _b)
      {
        return _a.zRange().max() < _b.zRange().max();
      }

      void insertUpperSlice(Slice const* _slice)
      {
        upperSlices_.push_back(_slice);
      }

      void insertLowerSlice(Slice const* _slice)
      {
        lowerSlices_.push_back(_slice);
      }

      link_cont_type upperSlices(int _numLayers)
      {
        link_cont_type _upperSlices = upperSlices_;

        for (int i = 1; i < _numLayers; ++i)
        {
          if (_upperSlices.empty()) break;
          link_cont_type _nextLayer;
          for (auto _slice : _upperSlices)
          {
            _nextLayer.insert(_nextLayer.end(),
                              _slice->upperSlices().begin(),
                              _slice->upperSlices().end());
          }
          _upperSlices = _nextLayer;
        }

        return _upperSlices;
      }

      link_cont_type lowerSlices(int _numLayers)
      {
        link_cont_type _lowerSlices = lowerSlices_;

        for (int i = 1; i < _numLayers; ++i)
        {
          if (_lowerSlices.empty()) break;
          link_cont_type _nextLayer;
          for (auto _slice : _lowerSlices)
          {
            _nextLayer.insert(_nextLayer.end(),
                              _slice->lowerSlices().begin(),
                              _slice->lowerSlices().end());
          }
          _lowerSlices = _nextLayer;
        }
        return _lowerSlices;
      }




      TBD_PROPERTY(range_type,zRange)
      TBD_PROPERTY_REF(SliceTag,tag)
      TBD_PROPERTY_REF_RO(link_cont_type,upperSlices)
      TBD_PROPERTY_REF_RO(link_cont_type,lowerSlices)
      TBD_PROPERTY_REF_RO(tooloutput_map_type,toolOutputs)
    };
  }
}
