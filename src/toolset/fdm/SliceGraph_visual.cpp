
#include <tomo/visual/gl.hpp>

#include <tomo/visual/gl/bounds.hpp>
#include "item/SliceGraph.hpp"
#include "visual/colorizer.hpp"
#include "visual/slice.hpp"
#include "visual/Path.hpp"
#include <tomo/visual/Outline.hpp>

#include <tomo/visual/Visualizer.hpp>

namespace tomo
{
  namespace fdm
  {
    namespace visual
    {
      using namespace tomo::visual;
      using gex::Range;

      class SliceGraph :
        public Visualizer<SliceGraph,item::SliceGraph>,
        private slice::Topology,
        private slice::Defect,
        private slice::Thin,
        private slice::Solid
      {
        enum Mode { NONE, SPEED, EXTRUSION_WIDTH, TAG } ;

        TOMO_PARAMETER_LIST
        (
          (bool) show_slices,
          (bool) show_paths,
          (bool) draw_range_only,
          (Range) range,
          (bool) show_bounds,

          /// Slices_and_paths
          (bool) solid_slices,
          (Color3) slice_color,

          // Analyze
          (bool) show_hole_topology,
          (Color3) hole_topology_color,
          (bool) show_slice_topology,
          (Color3) slice_topology_color,
          (bool) show_defect_slices,
          (Color3) defect_slice_color,
          (bool) show_thin_slices,
          (Color3) thin_slice_color,
          (float) graph_line_thickness,

          // Overlaps
          (bool) show_slice_overlaps_top,
          (bool) show_slice_overlaps_bottom,

          // Paths Only
          (int) mode,
          (Color3) path_color,
          (Color3) travel_color,
          (bool) solid_paths,
          (float) path_thickness,
          (bool) show_travel,
          (float) travel_thickness,
          (ColorRange3) extrusion_width,
          (ColorRange3) speed
        )

      public:
        typedef Visualizer<SliceGraph,item::SliceGraph> base_type;
        TOMO_VISUALIZER(item::SliceGraph)

        SliceGraph(base::Element const* _element) :
          base_type(_element),
          show_slices_(true),
          show_paths_(true),
          draw_range_only_(false),
          range_(0.0,20.0),
          show_bounds_(false),
          // Slices and paths parameters
          solid_slices_(true),
          slice_color_(Color3(1.0,1.0,0.0)),

          // Analyse
          show_hole_topology_(false),
          hole_topology_color_(Color3(1.0,0.0,1.0)),
          show_slice_topology_(false),
          slice_topology_color_(Color3(0.0,1.0,1.0)),
          show_defect_slices_(false),
          defect_slice_color_(Color3(1.0,0.0,0.0)),
          show_thin_slices_(false),
          thin_slice_color_(Color3(0.5,1.0,0.5)),
          graph_line_thickness_(1.0),

          mode_(int(NONE)),
          path_color_(Color3(0.0,0.5,1.0)),
          solid_paths_(true),
          path_thickness_(1.0),
          show_travel_(true),
          travel_color_(Color3(0.0,0.0,0.0)),
          travel_thickness_(1.0),
          extrusion_width_(gex::Range(0.0,1.0),Color3(0.0,0.0,0.0),Color3(1.0,1.0,1.0)),
          speed_(gex::Range(10.0,100.0),Color3(0.0,1.0,0.0),Color3(1.0,0.0,0.0)) 
        {}

        bounds_type bounds() const { return get()->data().bounds(); }

        void initialize()
        {
          fdm::SliceGraph const& _sliceGraph = this->get()->data();
          if (_sliceGraph.empty()) return; 

          slice::Topology::drawHoles(show_hole_topology());
          slice::Topology::drawSlices(show_slice_topology());
          slice::Topology::slicesColor(slice_topology_color());
          slice::Topology::holesColor(hole_topology_color());
          slice::Topology::preprocess(&_sliceGraph);

          slice::Solid::modified(true);
          slice::Thin::preprocess(&_sliceGraph);
          slice::Defect::preprocess(&_sliceGraph);
          slice::Solid::preprocess(&_sliceGraph);

          paths_.clear();
          pathsPreprocess(_sliceGraph);
          paths_.generateGeometry(_sliceGraph);
 
          if (!paths_.empty())
            solid_slices_ = false;
        }

        void preprocess()
        {
          fdm::SliceGraph const& _sliceGraph = this->get()->data();
          pathsPreprocess(_sliceGraph);
          slice::Topology::drawHoles(show_hole_topology());
          slice::Topology::drawSlices(show_slice_topology());
          slice::Topology::slicesColor(slice_topology_color());
          slice::Topology::holesColor(hole_topology_color());
          paths_.generateIndices();
        }

        void additionalParameters(tbd::Config& _cfg) const
        {
          using namespace tbd::parameter;

          make(*this,_cfg,"show_slices"); 
          if (show_slices())
          {
            make(*this,_cfg,"slice_color");
            make(*this,_cfg,"solid_slices");
            if (!solid_slices())
            {
              make(*this,_cfg,"graph_line_thickness");
            }  
          }

          make(*this,_cfg,"show_paths");
          

          if (show_paths())
          {
            make(_cfg,"mode",{
                { "type", "list"},
                { "items", "none,speed,extrusion_width,tag"} });

            if (mode() == NONE)
            {
              make(*this,_cfg,"path_color");
            }
            else if (mode() == SPEED)
            {
              make(*this,_cfg,"speed");
            }
            else if (mode() == EXTRUSION_WIDTH)
            {
              make(*this,_cfg,"extrusion_width");
            }

            make(*this,_cfg,"solid_paths");
            
            if (show_travel())
            {
              make(*this,_cfg,"path_thickness");
            }


            make(*this,_cfg,"show_travel"); 
            if (show_travel())
            {
              make(*this,_cfg,"travel_thickness");
            }
          }

          if (show_slices() || show_paths())
          {
            make(*this,_cfg,"draw_range_only");
            if (draw_range_only())
            {
              make(*this,_cfg,"range");
            }
          }
          make(*this,_cfg,"show_slice_topology");
          if (show_slice_topology())
            make(*this,_cfg,"slice_topology_color");
          
          make(*this,_cfg,"show_hole_topology");
          if (show_hole_topology())
            make(*this,_cfg,"hole_topology_color");

          make(*this,_cfg,"show_defect_slices");
          if (show_defect_slices())
            make(*this,_cfg,"defect_slice_color");
          
          make(*this,_cfg,"show_thin_slices");
          if (show_thin_slices())
            make(*this,_cfg,"thin_slice_color");

          make(*this,_cfg,"show_bounds");
        }

        void process()
        {
          // start point cache
          fdm::SliceGraph const& _sliceGraph = this->get()->data(); 
          if (_sliceGraph.empty()) return;

          glDisable(GL_LIGHTING);
          if (show_bounds())
          {
            gl::bounds(_sliceGraph.bounds(),Color4(1,1,1,1));
          }

          draw<slice::Topology>(show_hole_topology() || show_slice_topology());
          draw<slice::Thin>(show_thin_slices());
          draw<slice::Defect>(show_defect_slices());
          
          if (show_slices())
          {
            gl::color(slice_color());
            if (solid_slices())
            {
              glEnable(GL_LIGHTING);
              draw<slice::Solid>();
            }
            else
            {
              glLineWidth(graph_line_thickness_);
              tomo::slice::for_each(_sliceGraph,[&](const fdm::Slice& _slice)
              {
                auto _z = _slice.range();
                if (!gex::intersects(_z,Range(range_.min(),range_.max())) && draw_range_only()) return;
                visual::gl::outline(_slice.polygon(),_z);
              });
            }
          }

          if (show_paths())
          {
            paths_.draw(mode());
          }
        }

      private:
        visual::Path paths_; 

        template<typename T>
        void draw(bool _draw = true)
        {
          if (!_draw) return;
          if (draw_range_only())
          {
            T::draw(gex::Range(range_.min(),range_.max()));
          } else
          {
            T::draw();
          }
        }

        typedef gex::prim::Segment<gex::Point3> TopologyLink;
        std::vector<TopologyLink> topology_;

        typedef gex::base::Point<gex::base::Model<3,GLdouble>> tess_point;
        typedef std::vector<tess_point>::iterator tess_iterator;
 
        void pathsPreprocess(const fdm::SliceGraph& _sliceGraph)
        {
          paths_.useRange(draw_range_only());
          paths_.range() = range(); 
          paths_.drawTravel(show_travel());
          paths_.travelThickness(travel_thickness());
          paths_.pathThickness(path_thickness());
          paths_.pathColor(path_color());
          paths_.travelColor(travel_color());
          paths_.clearColors();
          paths_.addColorizer(EXTRUSION_WIDTH,_sliceGraph,colorizer::ExtrusionWidth(extrusion_width()));
          paths_.addColorizer(TAG,_sliceGraph,colorizer::Tag());
          paths_.addColorizer(SPEED,_sliceGraph,colorizer::Speed(speed()));
        }

      };
    }
  }
}

TOMO_VISUALIZER_PLUGIN_INTERFACE(tomo::fdm::visual::SliceGraph)
