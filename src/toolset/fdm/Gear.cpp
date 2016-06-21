#include <tomo/slice/extrude.hpp>
#include <tomo/item/Sink.hpp>
#include <tomo/tool/Tool.hpp>
#include "item/SliceGraph.hpp"

#include <gex/algorithm/difference.hpp>

namespace tomo
{
  namespace fdm
  {
    class Gear : public Tool<Gear>
    {
      TOMO_PARAMETER_LIST
      (
        (Sink<item::SliceGraph>) output,
        (float) height,
        (float) layer_height,
        (float) radius,
        (float) inner_radius,
        (Vec3) center,
        (int) number_of_teeth,
        (float) hole_radius
      )
    public:
      TOMO_TOOL("fdm",Gear)

      Gear(parent_type const* _parent) : 
        Tool<Gear>(_parent),
        output_(_parent),
        height_(1.0),
        layer_height_(0.3),
        radius_(10.0),
        inner_radius_(8.0),
        center_(gex::Vec3(0.0,0.0,0.0)),
        number_of_teeth_(20),
        hole_radius_(2.0) {}

      bool valid() const
      {
        return true;
      }

      void process()
      {

        using namespace tomo::slice;
        gex::Scalar _zPos = center_.z();
        auto&& _gear = gex::difference(gear(),hole());
        _gear.update();

        output_->layer_height() = layer_height();

        fdm::SliceGraph& _sliceGraph = output_->data();
        _sliceGraph.clear();
        extrude(_gear,strategy::Linear(height_,layer_height_,_zPos),_sliceGraph);
      }

      void additionalParameters(tbd::Config& _cfg) const
      {
        using namespace tbd::parameter;
        make(*this,_cfg,"height",{
            {"min","0.2"},
            {"max","100.00"}});
        make(*this,_cfg,"layer_height",{
            {"min","0.2"},
            {"max","1.0"}});
        make(*this,_cfg,"radius",{
            {"min","1.0"},
            {"max","1000.0"}});
        make(*this,_cfg,"hole_radius",{
            {"min","1.0"},
            {"max","1000.0"}});
        make(*this,_cfg,"inner_radius",{
            {"min","0.5"},
            {"max","500.0"}});
        make(*this,_cfg,"center");
        make(*this,_cfg,"number_of_teeth",{
            {"min","3"},
            {"max","1000"}});
      }

    private:
      gex::Ring gear()
      {
        gex::Ring _ring;
        Scalar _r1 = inner_radius();
        Scalar _r2 = radius();

        if (_r1 > _r2) std::swap(_r1,_r2);

        for (int i = 1; i <= number_of_teeth()*2; i++)
        {
          auto _radius = (i % 4 == 0 || i % 4 == 1) ? _r1 : _r2;
          Scalar _angle = Scalar(i)/Scalar(number_of_teeth()*2)*M_PI*2.0;
          Scalar _sin = _radius*sin(_angle),
                 _cos = _radius*cos(_angle);
          _ring.push_back(gex::Point2(_cos,_sin) + gex::Point2(center_.x(),center_.y()));
        }
        _ring.update();
        return _ring;
      }

      gex::Ring hole()
      {
        if (hole_radius() <= 0.0) return gex::Ring();
        gex::Ring _ring;
        const int _numSegments = 48;
        for (int i = 1; i <= _numSegments; i++)
        {
          Scalar _angle = Scalar(i)/Scalar(_numSegments)*M_PI*2.0;
          Scalar _sin = hole_radius()*sin(_angle),
                 _cos = hole_radius()*cos(_angle);
          _ring.push_back(gex::Point2(_cos,_sin) + gex::Point2(center_.x(),center_.y()));
        }
        _ring.update();
        return _ring;
      }
    };
  }
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::fdm::Gear)
