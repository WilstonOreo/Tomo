#pragma once

#include <memory>

namespace tomo
{
  namespace base
  {
    /// Standard pointer to an interface (is std::unique_ptr)
    template<typename T>
    using Ptr = std::unique_ptr<T>;
  }
}
