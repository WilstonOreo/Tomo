#pragma once

#include "detail.hpp"

namespace tomo
{
  namespace visual
  {
    namespace gl
    {
      template<GLenum MODE, typename PRIMITIVE, typename VERTEX_FUNCTOR>
      void primitive(const PRIMITIVE& _primitive, VERTEX_FUNCTOR f)
      {
        glBegin(GL_LINE_STRIP);
        for (auto& _p : _primitive) f(_p);
        glEnd();
      }

      template<typename PRIMITIVE, typename VERTEX_FUNCTOR>
      void primitive<GL_LINES>(const PRIMITIVE& _primitive, VERTEX_FUNCTOR f)
      {
        glBegin(GL_LINE_STRIP);
        for (auto& _seg : _primitive)
        {
          f(_seg[0]); f(_seg[1]);
        }
        glEnd();
      }

      template<typename POINTS, typename VERTEX_FUNCTOR>
      void points(const POINTS& _points, VERTEX_FUNCTOR f)
      {
        primitive<GL_POINTS>(_points,f);
      }

      template<typename LINESTRIP, typename VERTEX_FUNCTOR> 
      void line_strip(const LINESTRIP& _lineStrip, VERTEX_FUNCTOR f)
      {
        primitive<GL_LINE_STRIP>(_lineStrip,f);
      }

      template<typename LINELOOP, typename VERTEX_FUNCTOR>
      void line_loop(const LINELOOP& _lineLoop, VERTEX_FUNCTOR f)
      {
        primitive<GL_LINE_LOOP>(_lineLoop,f);
      }

      template<typename LINES, typename VERTEX_FUNCTOR>
      void lines(const LINES& _lines, VERTEX_FUNCTOR f)
      {
        primitive<GL_LINES>(_lines,f);
      }
    }
  }
}
