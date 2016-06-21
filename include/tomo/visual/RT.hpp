#pragma once

#include "DrawInterface.hpp"
#include "rt/Buffer.hpp"
#include "rt/intersect.hpp"
#include "rt/Color.hpp"
#include "rt/BlockGrid.hpp"
#include "rt/Intersection.hpp"
#include "rt/Light.hpp"
#include "rt/shader/Phong.hpp"

#include "tomo/misc/time.hpp"

namespace tomo
{
  namespace visual
  {
    /// Tomo Raytracer
    template<typename PRIMITIVES, int BLOCKSIZE = 16>
    struct RT
    {
      typedef PRIMITIVES primitives_type;
      typedef rt::DynamicBuffer buffer_type;
      typedef typename buffer_type::color_type color_type;
      typedef tomo::misc::Time time_type;
      typedef tomo::misc::Duration duration_type;
      typedef tomo::geometry::Ray3 ray_type;
      typedef typename PRIMITIVES::value_type primitive_type;
      typedef typename rt::Intersection<primitive_type> intersection_type;

      RT(const primitives_type& _primitives) : 
        primitives_(_primitives)
      {
      }

      void operator()(buffer_type& _buffer)
      {
        blockGrid_.resize(_buffer.width(),_buffer.height());
        typename grid_type::blockptr_cont_type _pendingBlocks;

        using tomo::misc::now;

        while (!_pendingBlocks.empty())
        {
          blockGrid_.getPendingBlocks(_pendingBlocks);
          size_t i = 0;

          auto _startTime = now();

          for (auto _block : _pendingBlocks)
          {
            auto&& _rays = _block->rays();
            auto&& _color = shade(_rays);
            _block->draw(_color);
            _block->nextPass();

            if (i % blocksPerUpdate() == 0)
            {
              if ((now() - _startTime).total_milliseconds() > updateTime().total_milliseconds())
              {
                blockGrid_.draw(_buffer);
                return;
              }
            }
              
            ++i;
          }
        }
      }

      TBD_PROPERTY_REF(duration_type,updateTime)
    private:

      template<typename RAY>
      void intersect(RAY& _ray)
      {
      }

      static constexpr size_t blocksPerUpdate() { return 256; }

      color_type shade(ray_type& _ray)
      {
        //typedef typename PRIMITIVES::value_type::shader_type;
        //shader_type _shader(_ray); 
        intersection_type _intersection;

        for (const auto& _p : primitives_)
        {
          auto _int = rt::intersect(_p,_ray);
          if (_int < _intersection) _intersection = _int;
        }
        return  _intersection.obj() ? color_type(255,255,255) : color_type(0,0,0);
      }

      std::vector<color_type> shade(std::vector<ray_type>& _rays)
      {
        std::vector<color_type> _colors;
        for (auto& _ray : _rays) 
          _colors.push_back(shade(_ray));
        return _colors;
      }

      typedef rt::BlockGrid<BLOCKSIZE> grid_type;

      grid_type blockGrid_;
      primitives_type const& primitives_; 
    };
  }
}
