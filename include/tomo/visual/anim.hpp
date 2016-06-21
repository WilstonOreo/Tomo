#include <boost/ptr_container/ptr_vector.hpp>
#include "tomo/defaults.h"

namespace tomo
{
  namespace visual
  {
    namespace scene
    {
      /// animation processing
      namespace anim
      {
        /** basic acceleration processor with infinite acceleraton
         * @tparam TIME time type to use
         */
        template<class TIME=DefTime> struct Acc
        {
          /** define local time type */
          typedef TIME Time;
          /** create acceleration processor */
          Acc( Time _start, Time _duration ) : start_(_start), duration_(_duration)
          {
          }
          Time elapsed(Time _time) const
          {
            if( finished(_time) )
              return duration_;
            else
              return _time - start_;
          }
          bool finished(Time _time) const
          {
            return _time >= start_ + duration_;
          }
          Time duration() const
          {
            return duration_;
          }
        protected:
          Time  start_;
          Time  duration_;
        };
        template<class TIME=DefTime> struct Linear : public Acc<TIME>
        {
          typedef TIME Time;
          typedef Acc<Time> Base;
          Linear( Time _start, Time _duration ) : Base(_start, _duration)
          {
          }
          template<class TYPE> TYPE progress( Time _time ) const
          {
            return Base::elapsed(_time) / Base::duration_;
          }
        };
        struct Objective
        {
          virtual void begin(double _progress ) = 0;
          virtual void end() = 0;
        };
        struct MatrixTransformation : Objective
        {
          virtual void begin(double _progress)
          {
            glPushMatrix();
            transform(_progress);
          }
          virtual void end()
          {
            glPopMatrix();
          }
        protected:
          virtual void transform( Type _progress ) = 0;
        };
        template<class COORD=DefCoord> struct Rotation : MatrixTransformation<COORD>
        {
          typedef TYPE Type;
          typedef MatrixTransformation<Type> Base;
          Rotation(Type _angle, Type _x, Type _y, Type _z)
            : angle_(_angle)
            , x_(_x), y_(_y), z_(_z)
          {
          }
          virtual void transform( Type _progress )
          {
            glRotate(_progress,x_,y_,z_);
          }
        protected:
          Type angle_, x_, y_, z_;
        };
        template<class TYPE=DEFAULT_TYPE> struct Objectives : Objective<TYPE>
        {
          typedef TYPE Type;
          typedef boost::ptr_vector<Objective<TYPE> > Container;
          typedef Objective<Type> Base;
          virtual void begin(const Type _progress)
          {
            for( typename Container::reference r : transformations_ )
              r.begin(_progress);
          }
          virtual void end()
          {
            for( typename Container::reference r : transformations_ )
              r.end();
          }
        protected:
          Container transformations_;
        };
      }
      typedef anim::Linear<> Linear;
      typedef anim::Rotation<> Rotation;
      namespace examples
      {
        void f()
        {
          Linear  acc(0.0,100.0);
          Rotation rot(180.0, 0.0, 0.0, 1.0);
          for( anim::DEFAULT_TIME i=0.0; i<acc.duration(); i += 1.0 )
          {
            anim::DEFAULT_TYPE progress=acc.progress<anim::DEFAULT_TIME>(i);
            rot.begin(progress);
            /// draw oject
            rot.end();
          }
        }
      }
    }
  }
}
