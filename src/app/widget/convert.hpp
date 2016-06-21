#pragma once
#include <tbd/qt/Converter.hpp>

namespace tomo
{
  namespace widget 
  {
    /// Namespace for functors to convert widgets from parameters and to convert widgets to tokens
    namespace convert
    {
      using tbd::qt::Converter;
      using tbd::qt::convert::Registrar;
      using tbd::qt::ParameterForm;
    }
  }
}

#include <tbd/qt/convert.hpp>
#include "convert/ColorButton.hpp"
#include "convert/Rotation.hpp"
#include "convert/Vector.hpp"
#include "convert/Range.hpp"
#include "convert/ColorRange.hpp"

