#define GL_GLEXT_PROTOTYPES

#include <tomo/visual/gl.hpp>
#include <tomo/visual/gl/bounds.hpp>
#include <tomo/visual/Visualizer.hpp>
#include <tomo/routing/bounds.hpp>
#include "item/Route.hpp"

#include "visual/colorizer.hpp"
#include "visual/Path.hpp"

namespace tomo
{
  namespace fdm
  {
    namespace visual
    {
      using namespace tomo::visual;

      class Route: 
        public Visualizer<Route,item::Route>
      {
        enum ColorMode {NONE,SPEED,EXTRUSION_WIDTH,TAG,FAN,TEMPERATURE};

        TBD_PARAMETER_LIST
        (
          (bool) draw_range_only,
          (Range) range,
          (bool) show_travel,
          (bool) show_events,
          (int) color_mode, /* color_modes : none, speed, extrusion_width, tag, fan, extruder_temp */
          (Color3) color,
          (Color3) travel_color,
          (bool) solid_paths,
          (float) path_thickness,
          (float) travel_thickness,
          (ColorRange3) extrusion_width,
          (ColorRange3) speed,
          (ColorRange3) temperature,
          (ColorRange3) fan
        )
      public:
        typedef Visualizer<Route,item::Route> base_type;
        TOMO_VISUALIZER(item::Route)

        Route(base::Element const* _element) :
          base_type(_element),
          draw_range_only_(false),
          range_(0.0,20.0),
          show_travel_(true),
          show_events_(false),
          color_mode_(NONE),
          color_(0.2,0.5,0.8),
          travel_color_(0.0,0.0,0.0),
          solid_paths_(true),
          path_thickness_(1.0),
          travel_thickness_(2.0),
          extrusion_width_(ColorRange3(gex::Range(0.0,1.0),Color3(0.0,0.0,0.0),Color3(1.0,1.0,1.0))),
          speed_(ColorRange3(gex::Range(10.0,100.0),Color3(0.0,1.0,0.0),Color3(1.0,0.0,0.0))),
          temperature_(ColorRange3(gex::Range(180.0,250.0),Color3(0.0,0.0,1.0),Color3(1.0,0.0,0.0))),
          fan_(ColorRange3(gex::Range(0.0,1.0),Color3(0,1,1),Color3(1,0.5,0)))
        {}

        void preprocess()
        {
          fdm::Route const& _route = this->get()->data();
          pathsPreprocess(_route);
          paths_.generateIndices();
        }

        void initialize()
        {
          fdm::Route const& _route = this->get()->data();
          paths_.clear();
          if (_route.empty()) return;
          
          pathsPreprocess(_route);
          paths_.generateGeometry(_route);
          bounds_ = tomo::routing::bounds(_route);
          preprocessEvents();
        }
        
        void process()
        {
          paths_.draw(color_mode());
          if (show_events())
            for (auto& _eObj : eventObjects_)
              _eObj.draw();
        }
        
        void additionalParameters(tbd::Config& _cfg) const
        {
          using namespace tbd::parameter;

          make(_cfg,"color_mode",
          {
            {"type","list"},
            {"items","none,speed,extrusion_width,tag,fan,temperature"}
          });

          switch(color_mode())
          {
            case NONE:
              make(*this,_cfg,"color");
              break;
            case SPEED:
              make(*this,_cfg,"speed");
              break;
            case EXTRUSION_WIDTH:
              make(*this,_cfg,"extrusion_width");
              break;
            case TAG:
              make(*this,_cfg,"tag");
              break;
            case TEMPERATURE:
              make(*this,_cfg,"temperature");
              break;
          };

          make(*this,_cfg,"draw_range_only");
          if (draw_range_only())
          {
            make(*this,_cfg,"range");
          }
          make(*this,_cfg,"show_travel");
          make(*this,_cfg,"show_events");
        }

        bounds_type bounds() const { return bounds_; }
      
      private:        
        void pathsPreprocess(const fdm::Route& _route)
        {
          paths_.useRange(draw_range_only());
          paths_.range() = range(); 
          paths_.drawTravel(show_travel());
          paths_.travelThickness(travel_thickness());
          paths_.pathThickness(path_thickness());
          paths_.pathColor(color());
          paths_.travelColor(travel_color());
          paths_.clearColors();
          paths_.addColorizer(EXTRUSION_WIDTH,_route,colorizer::ExtrusionWidth(extrusion_width()));
          paths_.addColorizer(TAG,_route,colorizer::Tag());
          paths_.addColorizer(SPEED,_route,colorizer::Speed(speed()));
          paths_.addColorizer(FAN,_route,colorizer::Fan(speed()));
          paths_.addColorizer(TEMPERATURE,_route,colorizer::ExtruderTemperature(temperature())); 
        }

        template<size_t NUM_VERTICES>
        struct EventObject
        {
          typedef std::array<gex::Point3,NUM_VERTICES> vertices_type;
        
          EventObject() {}
          EventObject(const fdm::Move& _move) 
          {
            generate(_move);
          }

          void generate(const fdm::Move& _move)
          {
            center_ = gex::Point3(_move.x(),_move.y(),_move.z());
            float _radius = 1.0;
            for (size_t i = 0; i < NUM_VERTICES; i++)
            {
              Scalar _angle = Scalar(i)/Scalar(NUM_VERTICES)*M_PI*2.0;
              vertices_[i] = gex::Point3(
                  _radius*sin(_angle),_radius*cos(_angle),0.0) + center_;
            }
          }

          void draw() const
          {
            glDisable(GL_LIGHTING);
            glColor4f(1.0,1.0,1.0,1.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex3f(center_.x(),center_.y(),center_.z());
            for (auto& _v : vertices_)
              glVertex3f(_v.x(),_v.y(),_v.z());
            auto& _v = vertices_.front();
            glVertex3f(_v.x(),_v.y(),_v.z());
            glEnd();
          }

          TBD_PROPERTY_REF_RO(vertices_type,vertices)
          TBD_PROPERTY_REF_RO(gex::Point3,center)
        };

        void preprocessEvents()
        {
          eventObjects_.clear();
          fdm::Route const& _route = this->get()->data();
        
          for (auto& _event : _route.events())
          {
            size_t _id = _event.first;
            if (_id >= _route.size()) _id = _route.size()-1;
            
            auto& _move = _route.path()[_id];
            if ((_move.z() < range_.min() || _move.z() > range_.max()) && draw_range_only())
              continue;
            
            eventObjects_.emplace_back(_move);
          }
        }

        std::vector<EventObject<8>> eventObjects_; 

        bounds_type bounds_;
        visual::Path paths_;
      };
    }
  }
}

TOMO_VISUALIZER_PLUGIN_INTERFACE(tomo::fdm::visual::Route)
