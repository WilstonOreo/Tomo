#pragma once

#include "Move.hpp"
#include "slice.hpp"

namespace tomo
{
  namespace fdm
  {
    struct MoveMaker
    {
      typedef Move::pos_type pos_type;
      typedef Move::speed_type speed_type;
      typedef gex::Scalar scalar_type;
      
      template<typename...ARGS>
      MoveMaker(const Slice& _slice, ARGS&&..._args) : 
        slice_(_slice),
        move_(pos_type(),_args...) {}

      template<typename POINT>
      void operator()(const POINT& _point, Move& _move)
      {
        _move = move_;
        using namespace gex;
        _move(X) = _point.x();
        _move(Y) = _point.y();
        _move(Z) = slice_.range().max();
      }

    private:
      Slice const& slice_;
      Move move_;
    };
  }
}
