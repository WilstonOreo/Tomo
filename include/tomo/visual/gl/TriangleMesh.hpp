#pragma once

#include "Draw.hpp"
#include <gex/comp.hpp>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "Base.hpp"

namespace tomo
{
  namespace visual
  {
    namespace gl
    {
      template<>
      struct Draw<gex::TriangleMesh>
      {
        inline void operator()(const gex::TriangleMesh& _mesh)
        {
          gl::triangles(_mesh.triangles(),[&](const gex::Triangle& _t)
          {
            gl::Normal(_t.normal());
            for (auto& _p : _t.points())
              gl::Vertex(_p);
          });
        }
      };
    }
  }
}

