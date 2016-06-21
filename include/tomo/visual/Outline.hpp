#pragma once

#include <gex/algorithm/for_each.hpp>

namespace tomo
{
  namespace visual
  {
    namespace gl
    {
      template<typename POLYGON>
      void outline(const POLYGON& _polygon)
      {
        typedef typename POLYGON::ring_type ring_type;

        glEnableClientState(GL_VERTEX_ARRAY);
        gex::for_each<ring_type>(_polygon,[&](const ring_type& _ring)
        {
          if (_ring.empty()) return;
          glVertexPointer(2,GL_FLOAT,0,_ring.data());
          glDrawArrays(GL_LINE_LOOP, 0, _ring.size());
        });
        glDisableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

      }

      template<typename POLYGON, typename ZPOS>
      void outline(const POLYGON& _polygon, ZPOS _zPos)
      {
        glTranslatef(0,0,_zPos.middle());
        outline(_polygon);
        glTranslatef(0,0,-_zPos.middle());
      }
    }
  }
}
