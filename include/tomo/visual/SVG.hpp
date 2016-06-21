#pragma once

#include <gex/prim.hpp>
#include <gex/io.hpp>

namespace tomo
{
  namespace visual
  {
    namespace svg
    {
      typedef gex::io::svg::Buffer Buffer;      
      typedef gex::io::svg::Style Style;

      template<typename PRIMITIVE>
      struct Draw
      {
        void operator()(const PRIMITIVE& _p, 
                        const Style& _style, 
                        Buffer& _buffer)
        {
          gex::io::svg::Draw<PRIMITIVE>()(_p,_style,_buffer);
        }
      };
    }

    typedef gex::io::DrawInterface<svg::Draw, svg::Buffer> SVG;
  }
}
