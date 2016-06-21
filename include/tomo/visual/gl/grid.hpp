#pragma once

namespace tomo
{
  namespace visual
  {
    namespace gl
    {
      template<typename BOUNDS, typename COLOR_FINE, typename COLOR_RAW>
      void grid( const BOUNDS& _bounds, const COLOR_FINE& _colorFine, const COLOR_RAW& _colorRaw)
      {
        // draw grid
        float _tick=1.0;
        {
          // lines X axis
          glLineWidth( 1.0 );
          glBegin(GL_LINES);
          {
            int i = 0;
            for (float _pos = _bounds.min().x(); _pos <= _bounds.max().x(); _pos += _tick, ++i)
            {
              auto& _color = i % 10 ? _colorFine : _colorRaw;
              glColor4f(_color.r(),_color.g(),_color.b(),_color.a()*(i%10)?0.5:1.0);
              glVertex3f(_pos, _bounds.min().y(), _bounds.min().z());
              glVertex3f(_pos, _bounds.max().y(), _bounds.min().z());
            }
          }
          glEnd();
          // lines Y axis
          glBegin(GL_LINES);
          {
            int i = 0;
            for (float _pos = _bounds.min().y(); _pos <= _bounds.max().y(); _pos += _tick, ++i)
            {
              auto& _color = i % 10 ? _colorFine : _colorRaw;
              glColor4f(_color.r(),_color.g(),_color.b(),_color.a()*(i%10)?0.5:1.0);
              glVertex3f(_bounds.min().x(), _pos, _bounds.min().z());
              glVertex3f(_bounds.max().x(), _pos, _bounds.min().z());
            }
          }
          glEnd();
        }
      }
    }
  }
}
