#pragma once

#include "Color.hpp"
#include "gl.hpp"
#include <gex/base.hpp>
#include "Visualizer.hpp"
#include <tomo/item/Mesh.hpp>
#include "gl/bounds.hpp"
#include "ColorRange.hpp"

namespace tomo
{
  namespace visual
  {
    class Mesh : 
      public Visualizer<Mesh,item::Mesh>
    {
      TBD_PARAMETER_LIST
      (
        (Color3) color,
        (bool) draw_bounds,
        (bool) draw_transformed_bounds
      )
    public:
      TOMO_VISUALIZER(item::Mesh)

      Mesh(Element const* _mesh) : 
        Visualizer<Mesh,item::Mesh>(_mesh),
        color_(1.0,1.0,1.0),
        draw_bounds_(false),
        draw_transformed_bounds_(false) {}
     
      inline bounds_type bounds() const
      {
        return this->get()->transformedBounds();
      }

      inline void process()
      {
        glDisable(GL_LIGHTING);
        auto& _meshItem = *this->get();
        gex::TriangleMesh const& _mesh = _meshItem.data();

        using namespace gl; 
        
        auto _center = _mesh.bounds().center();
        auto& _offset = _meshItem.offset();

        
        if (draw_transformed_bounds())
        {
          gl::bounds(_meshItem.transformedBounds(),Color3(1,1,1));
        }
        glPushMatrix();
        glLoadIdentity();
        
        translate(_center);
        translate(_offset);
        rotate(_meshItem.rotate().z(),gex::Z);
        rotate(_meshItem.rotate().y(),gex::Y);
        rotate(_meshItem.rotate().x(),gex::X);
        scale(_meshItem.scale());
        translate(gex::Vec3(-_center));
       
        if (draw_bounds())
        {
          gl::bounds(_mesh.bounds(),Color3(1,1,1));
        }

        glEnable(GL_LIGHTING);
        gl::color(color_);
        draw(GL_TRIANGLES,[&]()
        {
          for (auto& _t : _mesh.triangles())
          {
            normal(_t.normal());
            for (auto& _p : _t.points())
              vertex(_p);
          }
        });
        glPopMatrix();
      }
    };
  }
}

