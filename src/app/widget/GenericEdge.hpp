#pragma once

#include <gex/qt.hpp>

namespace tomo
{
  namespace widget
  {
    template<typename SRC, typename DEST>
    class GenericEdge
    {
    public:
      typedef SRC source_type;
      typedef DEST dest_type;

      GenericEdge(
        source_type* _source,
        dest_type* _dest, float _width = 1.0) :
        source_(_source),
        dest_(_dest),
        sourcePoint_(0,0),
        destPoint_(0,0),
        width_(_width) {}

      bool valid() const
      {
        return dest_ && source_;
      }

      gex::Point2 dir() const
      {
        if (!valid()) gex::Point2(0.0,0.0);
        return (destPoint() - sourcePoint()).normalized();
      }

      gex::Point2 normal() const
      {
        auto _dir = dir();
        return gex::Point2(-_dir.y(),_dir.x());
      }

      float distance() const
      {
        if (!valid()) return 0.0;
        return (destPoint() - sourcePoint()).norm();
      }

      virtual void adjust() = 0;

      TBD_PROPERTY(source_type*,source)
      TBD_PROPERTY(dest_type*,dest)
      TBD_PROPERTY_REF(gex::Point2,sourcePoint)
      TBD_PROPERTY_REF(gex::Point2,destPoint)
      TBD_PROPERTY_REF_RO(float,width)
    protected:
      void setPoints(
        const QPointF& _sourcePoint,
        const QPointF& _destPoint)
      {
        sourcePoint_ = gex::qt::adapt(_sourcePoint);
        destPoint_ = gex::qt::adapt(_destPoint);
      }

      void setPoints(
        const gex::Point2& _sourcePoint,
        const gex::Point2& _destPoint)
      {
        sourcePoint_ = _sourcePoint;
        destPoint_ = _destPoint;
      }

      gex::Bounds2 boundingRect() const
      {
        if (!valid()) return gex::Bounds2();
        float _extra = width_ / 2.0;
        gex::Point2 _d(_extra,_extra);

        gex::Bounds2 _bounds;
        _bounds.extend(sourcePoint_);
        _bounds.extend(destPoint_);
        _bounds.min() -= _d;
        _bounds.max() += _d;
        return _bounds;
      }
    };
  }
}
