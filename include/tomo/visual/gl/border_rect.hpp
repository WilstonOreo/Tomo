#pragma once

#include <tomo/visual/gl.hpp>

namespace tomo
{
  namespace visual
  {
    namespace gl
    {
      template<typename BOUNDS, typename WIDTH, typename COLOR>
      void border_rect(
        const BOUNDS& _bounds,
        const WIDTH& _width,
        const COLOR& _color )
      {
        using namespace tbd::gl;
        // draw _bounds.min().
        color(_color);
        draw(GL_QUADS,[&]()
        {
          vertex(_bounds.min().x(), _bounds.min().y()+_width, _bounds.min().z());
          vertex(_bounds.min().x(), _bounds.min().y(), _bounds.min().z());
          vertex(_bounds.max().x()-_width, _bounds.min().y(), _bounds.min().z());
          vertex(_bounds.max().x()-_width, _bounds.min().y()+_width, _bounds.min().z());
          vertex(_bounds.min().x()+_width, _bounds.max().y(), _bounds.min().z());
          vertex(_bounds.min().x()+_width, _bounds.max().y()-_width, _bounds.min().z());
          vertex(_bounds.max().x(), _bounds.max().y()-_width, _bounds.min().z());
          vertex(_bounds.max().x(), _bounds.max().y(), _bounds.min().z());
          vertex(_bounds.min().x(), _bounds.min().y()+_width, _bounds.min().z());
          vertex(_bounds.min().x()+_width, _bounds.min().y()+_width, _bounds.min().z());
          vertex(_bounds.min().x()+_width, _bounds.max().y(), _bounds.min().z());
          vertex(_bounds.min().x(), _bounds.max().y(), _bounds.min().z());
          vertex(_bounds.max().x()-_width, _bounds.min().y(), _bounds.min().z());
          vertex(_bounds.max().x(), _bounds.min().y(), _bounds.min().z());
          vertex(_bounds.max().x(), _bounds.max().y()-_width, _bounds.min().z());
          vertex(_bounds.max().x()-_width, _bounds.max().y()-_width, _bounds.min().z());
        });
      }
    }
  }
}
