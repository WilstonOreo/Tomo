#pragma once

#include "Buffer.hpp"

namespace tomo
{
  namespace visual
  {
    namespace gl
    {
      template<typename PRIMITIVE>
      struct Draw
      {
      };
    
      template<typename PRIMITIVE>
      void draw(const PRIMITIVE& _primitive)
      {
        Draw<PRIMITIVE>()(_primitive);
      }
    }
  }
}

