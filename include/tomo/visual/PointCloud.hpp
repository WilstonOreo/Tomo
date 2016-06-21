#pragma once

#include <gex/base.hpp>
#include "Interface.hpp"
#include <tomo/item/PointCloud.hpp>
#include "gl/Base.hpp"
#include "Bounds.hpp"

namespace tomo
{
  namespace visual
  {
    class PointCloud : 
      public SerializerWithBase<PointCloud,Visualizer<item::PointCloud>>,
      public tool::Registrar<PointCloud>
    {
      TBD_PARAMETER_LIST
      (
        (Color3) color,
        (Float) point_size,
        (Bool) bounds,
        (Bool) transformed_bounds
      )
    public:
      typedef Visualizer<item::PointCloud> base_type;
      typedef SerializerWithBase<PointCloud,base_type> serializer_type;
      TOMO_VISUALIZER(PointCloud)

      PointCloud(mutable_parent_type _parent) : 
        serializer_type(*this,_parent),
        color_(1.0,1.0,1.0),
        point_size_(1.0), 
        bounds_(true),
        transformed_bounds_(true) {}
      
      inline void process()
      {
        glDisable(GL_LIGHTING);
        
        auto _pcItem = this->get();
        gex::PointCloud const& _pointCloud = *_pcItem;

        glPushMatrix();
        auto _center = _pointCloud.bounds().center();
        auto& _offset = _pcItem->offset();
        
        if (transformed_bounds())
        {
          drawBounds(_pcItem->transformedBounds(),Color3(1,1,1));
        }

        glTranslatef(_center.x(),_center.y(),_center.z());
        glTranslatef(_offset.x(),_offset.y(),_offset.z());
        glRotatef(_pcItem->rotate_z(),0.0,0.0,1.0);
        glRotatef(_pcItem->rotate_y(),0.0,1.0,0.0);
        glRotatef(_pcItem->rotate_x(),1.0,0.0,0.0);
        glScalef(_pcItem->scale().x(),_pcItem->scale().y(),_pcItem->scale().z());
        glTranslatef(-_center.x(),-_center.y(),-_center.z());
       
        if (bounds())
        {
          drawBounds(_pointCloud.bounds(),Color3(1,1,1));
        }

        glPointSize(point_size());
        gl::Color(color_); 
        gl::points(_pointCloud.points(),[&](const gex::Point3& _p)
        {
          gl::Vertex(_p);
        });
        glEnable(GL_LIGHTING);
        glPopMatrix();
      }
    };
  }
}

