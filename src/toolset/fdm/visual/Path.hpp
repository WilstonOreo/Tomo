#pragma once

#include <tomo/visual/gl/VBO.hpp>
#include "../routing/slice.hpp"
#include "../routing/routing.hpp"
#include <tomo/slice/for_each.hpp>

namespace tomo
{
  namespace fdm
  {
    namespace visual
    {
      typedef gex::Range Range;
      using namespace tomo::visual;

      namespace detail
      {
        template<typename POINT, typename THICKNESS>
        gex::Vec3 getNormal(const POINT& _back, const POINT& _front, THICKNESS _t)
        {
          gex::Vec3 _d = _front - _back;
          if (_d.norm() <= _t*0.01)
          {
            return gex::Vec3(0.0,_t * 0.5,0.0);
          }
          gex::Vec3 _normal(-_d.y(),_d.x(),0.0);
          _normal.normalize();
          _normal *= _t * 0.5;
          return _normal;
        }

        template<typename HEIGHT>
        gex::Vec3 getUpVec(HEIGHT _height)
        {
          return gex::Vec3(0.0,0.0,_height * 0.5);
        }

        template<typename POINT, typename THICKNESS>
        gex::Vec3 getDirection(const POINT& _back, const POINT& _front, THICKNESS _t)
        {
          gex::Vec3 _d = _front - _back;
          if (_d.norm() <= _t*0.01)
          {
            return gex::Vec3(_t * 0.5,0.0,0.0);
          }
          return _t *0.5 * _d.normalized();
        }

        template<typename POINT,
                 typename SCALAR,
                 typename VERTICES,
                 typename NORMALS>
        void generateSolidSegment(
          const POINT& _back,
          const POINT& _front,
          SCALAR _t,
          SCALAR _height,
          VERTICES& _vertices,
          NORMALS& _normals)
        {
          auto&& _up = getUpVec(_height);
          auto&& _normal = getNormal(_back,_front,_t);
          auto&& _d = getDirection(_back,_front,_t);
          auto makeFrontBackVertexPair = [&](const gex::Vec3& _n)
          {
            for (bool _frontOrBack : {false,true})
            {
              _vertices.emplace_back((_frontOrBack ? _front : _back) + _n);
              _normals.emplace_back(_n);
            }
          };
                  
          makeFrontBackVertexPair(_normal);
          makeFrontBackVertexPair(_up);
          makeFrontBackVertexPair(-_normal);
          makeFrontBackVertexPair(-_up);
          _vertices.emplace_back(_back - _d);
          _normals.emplace_back(-_d);
          _vertices.emplace_back(_front + _d);
          _normals.emplace_back(_d); 
        }
      }



      struct Path
      {
        Path() :
          useRange_(false),
          pathColor_(1.0,1.0,1.0),
          pathThickness_(1.0),
          drawTravel_(true),
          travelThickness_(1.0),
          travelColor_(0.0,0.0,0.0) {}

        ~Path()
        {
          clear();
        }

        void clear()
        {
          vertices_.clear();
          normals_.clear();
          travels_.clear();
          indices_.clear();
          clearColors();
        }

        void clearColors()
        {
          colors_.clear();
        }

        bool empty() const
        {
          return vertices_.empty();
        }

        inline void generateGeometry(const fdm::Path& _path, float _height)
        {
          typedef std::pair<Move,Move> move_pair;
          gex::for_each<move_pair>(_path,[&](const Move& _first, const Move& _second)
          {
            append(_first,_second,_height);
          });
          generateIndices();
        }
        
        inline void generateGeometry(const fdm::SliceGraph& _sliceGraph)
        { 
          tomo::slice::for_each(_sliceGraph,[&](const Slice& _slice)
          {
            for (auto& _path : _slice.paths())
            {
              generateGeometry(_path,_slice.height());
            }
          });
          generateIndices();
        }

        inline void generateGeometry(const fdm::Route& _route)
        {
          routing::iterate(_route,tomo::routing::strategy::State(),[&](const State& _lastState, const State& _state)
          {
            append(_lastState,_state,_state.layerHeight());
          });
          generateIndices();
        }

        template<typename COLORIZER>
        void addColorizer(int _id, const fdm::Path& _path, COLORIZER _c, bool _clear = false)
        {
          auto& _colors = colors_[_id];
          if (_clear) _colors.clear();
          _colors.reserve(_colors.size() + _path.size());
          typedef std::pair<Move,Move> move_pair;
          gex::for_each<move_pair>(_path,[&](const Move& _first, const Move& _second)
          {
            if (_first.travel()) return;
            auto&& _color = _c(_first);

            /// Repeat colors so that they are aligned with the vertex arrays 
            for (int i = 0; i < 10; ++i)
            {
              _colors.push_back(_color);
            }
          });
        }

        template<typename COLORIZER>
        void addColorizer(int _id, const fdm::SliceGraph& _sliceGraph, COLORIZER _c)
        {
          tomo::slice::for_each(_sliceGraph,[&](const Slice& _slice)
          {
            for (auto& _path : _slice.paths())
            {
              addColorizer(_id,_path,_c);
            }
          });
        }

        template<typename COLORIZER>
        void addColorizer(int _id, const fdm::Route& _route, COLORIZER _c)
        {
          auto& _colors = colors_[_id];
          _colors.reserve(_colors.size() + _route.size()*10);

          routing::iterate(_route,tomo::routing::strategy::State(),[&](const State& _lastState, const State& _state)
          {
            if (_lastState.travel()) return;
            auto&& _color = _c(_lastState);

            /// Repeat colors so that they are aligned with the vertex arrays 
            for (int i = 0; i < 10; ++i)
            {
              _colors.push_back(_color);
            }
          });
        }


        inline void draw(int _id = 0)
        {
          if (colors_.count(_id))
          {
            auto& _colors = colors_[_id];
            /// Draw solid paths
            glEnable(GL_LIGHTING);
            glEnableClientState(GL_COLOR_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);

            glColorPointer(4, GL_UNSIGNED_BYTE, 0, _colors.data());
            glNormalPointer(GL_FLOAT, 0, normals_.data());
            glVertexPointer(3, GL_FLOAT, 0, vertices_.data());
            glDrawElements(GL_TRIANGLES,indices_.size(),GL_UNSIGNED_INT,indices_.data());

            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
          } else
          {
            gl::color(pathColor());
            /// Draw solid paths
            glEnable(GL_LIGHTING);
            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);

            glNormalPointer(GL_FLOAT, 0, normals_.data());
            glVertexPointer(3, GL_FLOAT, 0, vertices_.data());
            glDrawElements(GL_TRIANGLES,indices_.size(),GL_UNSIGNED_INT,indices_.data());

            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
          }

            glDisable(GL_LIGHTING);
            if (drawTravel())
            {
              gl::color(travelColor());
              glEnableClientState(GL_VERTEX_ARRAY);
              glVertexPointer(3, GL_FLOAT, 0, travels_.data());
              glDrawElements(GL_LINES,travelIndices_.size(),GL_UNSIGNED_INT,travelIndices_.data());
              glDisableClientState(GL_VERTEX_ARRAY);
            }
        }

        inline void generateIndices()
        {
          static std::array<GLuint,48> _ind( 
              {
                /// Side triangles
                0,1,2,
                1,2,3,
                2,3,4,
                3,4,5,
                4,5,6,
                5,6,7,
                6,7,0,
                7,1,2,

                /// Cap front
                8,0,2,
                8,2,4,
                8,4,6,
                8,6,0,

                /// Cap back
                9,1,3,
                9,3,5,
                9,5,7,
                9,7,1
              });
          static constexpr int _verticesPerSegment = 10;
          indices_.clear();
          indices_.reserve(vertices_.size()* _ind.size() / _verticesPerSegment);

          for (int _segment = 0; _segment < vertices_.size(); _segment += _verticesPerSegment)
          {
            // Find path segments which lie in a certain range
            bool _inRange = !useRange();
            if (!_inRange)
            {
              for (int _vertex = _segment; _vertex < _segment + _verticesPerSegment; ++_vertex)
              {
                _inRange |= gex::intersects(vertices_[_vertex].z(),range_);
                if (_inRange) break;
              }
              if (!_inRange) continue;
            }
            auto _tmp = _ind;
            for (auto& _t : _tmp) 
              _t += _segment;
            indices_.insert(indices_.end(),_tmp.begin(),_tmp.end());
          }

          /// Travel indices
          travelIndices_.clear();
          travelIndices_.reserve(travels_.size());
          static constexpr int _verticesPerTravel = 2;

          for (int _segment = 0; _segment < travels_.size(); _segment += _verticesPerTravel) 
          {
            auto& _front = travels_[_segment];
            auto& _back = travels_[_segment+1];
            bool _inRange = !useRange() || 
              gex::intersects(_front.z(),range_) ||
              gex::intersects(_back.z(),range_);  
            if (_inRange)
            {
              travelIndices_.push_back(_segment);
              travelIndices_.push_back(_segment+1);
            }
          }
        }

        TBD_PROPERTY(bool,useRange)
        TBD_PROPERTY_REF(gex::Range,range)
        TBD_PROPERTY_REF(Color3,pathColor) 
        TBD_PROPERTY_REF(float,pathThickness)
        TBD_PROPERTY(bool,drawTravel)
        TBD_PROPERTY_REF(float,travelThickness)
        TBD_PROPERTY_REF(Color3,travelColor)
      private:
        template<typename STATE_OR_MOVE, typename HEIGHT>
        void append(const STATE_OR_MOVE& _first, const STATE_OR_MOVE& _second, HEIGHT _height)
        {
          typedef typename STATE_OR_MOVE::pos_type pos_type;
          if (!_first.travel())
          {
            detail::generateSolidSegment(pos_type(_first),pos_type(_second),_first.extrusionWidth(),_height,vertices_,normals_);
          }
          else
          {
            travels_.push_back(_first);
            travels_.push_back(_second);
          }
        }

        std::vector<gex::Vec3> vertices_;
        std::vector<gex::Point3> normals_;
        std::vector<gex::Point3> travels_;
        std::vector<Color4ub> travelColors_;
        std::map<int,std::vector<Color4ub>> colors_;
        std::vector<GLuint> indices_;
        std::vector<GLuint> travelIndices_;
      };
    }
  }
}
