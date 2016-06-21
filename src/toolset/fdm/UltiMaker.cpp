#include <tomo/routing/generate.hpp>
#include <tomo/routing/gcode/write.hpp>
#include <tomo/item/Sink.hpp>
#include <tomo/item/Source.hpp>
#include <tomo/item/FileSink.hpp>

#include <tomo/device/Device.hpp>
#include "item/Route.hpp"
#include "routing/Event.hpp"
#include "routing/Extruder.hpp"
#include "routing/GCode.hpp"

namespace tomo
{
  namespace fdm
  {
    using gex::Range;
    using gex::Bounds3;
    using gex::Vec3;

    class UltiMaker :
      public Device<UltiMaker>
    {
      TOMO_PARAMETER_LIST
      (
        (Source<item::Route>) source,
        (FileSink) output_file,
        (float) retract_speed,
        (Range) speed_range,
        (Range) extrusion_width_range,
        (Range) temperature_range,
        (Range) layer_height_range,
        (Bounds3) print_bounds,
        (float) filament_diameter,
        (float) extrusion_multiplier,
        (float) nozzle_diameter,
        (float) bed_z_offset,
        (float) retract_distance,
        (float) retraction,
        (int) num_extruders
      )

      GCodes gcodes_;
    public:
      TOMO_DEVICE("fdm",UltiMaker)

      UltiMaker(parent_type const* _parent) :
        Device<UltiMaker>(_parent),
        retract_speed_(30.0),
        speed_range_(5,200),
        extrusion_width_range_(0.1,1.0),
        temperature_range_(150,300),
        layer_height_range_(0.1,0.4),
        print_bounds_(Vec3(-100,-100,0),Vec3(100,100,200)),
        filament_diameter_(1.75),
        extrusion_multiplier_(1.0),
        nozzle_diameter_(0.35),
        bed_z_offset_(0.1),
        retract_distance_(5.0),
        retraction_(1.0),
        num_extruders_(1) {}

      bool valid() const
      {
        return source();
      }
      
      template<typename T>
      T extrusionWidth(T _extrusionWidth) const
      {
        return (_extrusionWidth <= extrusion_width_range().min()) ? 
          extrusion_width_range().min() : 
          _extrusionWidth;
      }

      template<typename T>
      T speed(T _speed) const
      {
        if (_speed < speed_range().min() ||
            _speed > speed_range().max()) return speed_range().min();
        return _speed;
      } 
      
      template<typename T>
      T temperature(T _temperature) const
      {
        if (_temperature < temperature_range().min() ||
            _temperature > temperature_range().max()) return temperature_range().min();
        return _temperature;
      }

      template<typename T>
      T layer_height(T _layer_height) const
      {
        if (_layer_height < layer_height_range().min() ||
            _layer_height > layer_height_range().max()) return layer_height_range().min();
        return _layer_height;
      }

      void process()
      {
        fdm::Route const& _route = source()->data();
  
        std::ofstream _stream(output_file_.path());

        typedef Extruder<UltiMaker> Extruder;
        typedef fdm::StateIncrementer<Extruder> StateIncrementer;
        Extruder _extruder(*this);
        StateIncrementer _inc(_extruder);
        gcode::Generator _generator(gcodes_);
        
        tomo::routing::iterate(_route,
            tomo::routing::strategy::StateWithIncrementer<StateIncrementer>(_inc),[&](const State& _prev, const State& _next)
        {
          _generator.initialization(_prev,_next); 

          if (_next.retract())
          {
            _extruder.snort();
            _generator.controlled_movement(Speed(retract_speed()),_extruder.position());
            _extruder.reset();
          }

          _generator.fan(_prev,_next);
          _generator.temperature(_prev,_next);
          
          if (_extruder.origin())
          {
            _generator.set_position(_extruder.position());
          
            if (!_prev.travel())
            {
              _extruder.squirt();
              _generator.controlled_movement(Speed(retract_speed()),_extruder.position());
              _generator.controlled_movement(Speed(_prev.speed()));
            }
          }

          _extruder.extrude(_prev,_next,_next.layerHeight());
          _generator.controlled_movement(_prev,_next,_extruder.position());
          _generator.finalization(_prev,_next);

          _stream << _generator;
          _generator.clear();
        });
      }
      
      void saveToFile(const std::string& _filename) 
      {
        output_file_.path(_filename);
        process();
      }
    }; 
  }
}

#include <tomo/visual/gl.hpp>
#include <tomo/visual/gl/border_rect.hpp>
#include <tomo/visual/gl/grid.hpp>
#include <tomo/visual/Visualizer.hpp>

namespace tomo
{
  namespace fdm
  {
    namespace visual
    {
      using namespace tomo::visual::gl;

      class UltiMaker : public Visualizer<UltiMaker,fdm::UltiMaker>
      {
        TOMO_PARAMETER_LIST
        (
          (float) grid_resolution,
          (bool) draw_bed,
          (float) bed_border_width
        )
      public:
        TOMO_VISUALIZER(fdm::UltiMaker)
        
        UltiMaker(base::Element const* _element) :
          Visualizer<UltiMaker,fdm::UltiMaker>(_element),
          grid_resolution_(10),
          draw_bed_(true),
          bed_border_width_(5) {}

        bounds_type bounds() const { return get()->print_bounds(); }

        void process()
        {  
          if (draw_bed())
          {
            auto _printBounds = bounds();
            disabled(GL_LIGHTING,[&]()
            {
              border_rect(_printBounds,bed_border_width(),Color4(0.5,0.5,0.5,0.5));
              
              auto _gridBounds = _printBounds;
              float _b = bed_border_width();
              _gridBounds.min() += Vec3(_b,_b,0);
              _gridBounds.max() -= Vec3(_b,_b,0);
              grid(_gridBounds,Color4(0.3,0.3,0.3),Color4(0.5,0.5,0.5));
            });
          }
        }
      };
    }
  }
}

#include <tomo/query/Inquirer.hpp>
#include "query/Path.hpp"

namespace tomo
{
  namespace fdm
  {
    namespace query
    {
      class UltiMaker : public Inquirer<UltiMaker,fdm::UltiMaker>
      {
        TOMO_PARAMETER_LIST
        (
          (bool) path_information,
          (bool) state_information
        )
      public:
        TOMO_INQUIRER(fdm::UltiMaker)

        UltiMaker(base::Element const* _element) :
          Inquirer<UltiMaker,fdm::UltiMaker>(_element),
          path_information_(true),
          state_information_(true) {}

        void preprocess()
        {
          if (!this->get()->source()) return;

          fdm::Route const& _route = this->get()->source()->data();
          data().clear();
          pathQuery_(_route);

          using namespace tomo::routing;          
          float _travelLength = 0.0;
          State _finalState;
          typedef Extruder<fdm::UltiMaker> Extruder;
          typedef fdm::StateIncrementer<Extruder> StateIncrementer;
          Extruder _extruder(*this->get());
          StateIncrementer _inc(_extruder);

          iterate(_route,tomo::routing::strategy::StateWithIncrementer<StateIncrementer>(_inc),[&](const State& _prev, const State& _next)
          {
            _finalState = _next;
            if (_prev.travel())
            {
              _travelLength += gex::distance(_prev,_next);
            } else
            {
              _extruder.extrude(_prev,_next,_next.layerHeight());  
            }
          });

          filamentLength_ = _finalState.filament_length(); 
          travelLength_ = _travelLength;
        }

        void process()
        {
          typedef tbd::ConfigPath path_type;
          if (path_information())
            pathQuery_.save(path_type("path"),data());
          data().put(path_type("filament_length"),filamentLength_); 
          data().put(path_type("travel_length"),travelLength_); 
        }

      private:
        float filamentLength_;
        float travelLength_;
        query::Path pathQuery_;
      };
    }
  }
}


extern "C" 
{ 
  void reg()
  { 
    tomo::device::Registry::reg<tomo::fdm::UltiMaker>(); 
    tomo::visual::Registry::reg<tomo::fdm::visual::UltiMaker>(); 
    tomo::query::Registry::reg<tomo::fdm::query::UltiMaker>(); 
  }
}
