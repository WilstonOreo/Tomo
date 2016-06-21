#pragma once

#include "PtrType.hpp"

namespace tomo
{
  namespace base
  {
    namespace util
    {
      namespace functor
      {
        /// Util functor for determining if a pointer is not nullptr
        template<typename T>
        struct Valid
        {
          bool operator()(const T& _t) const
          {
            return false;
          }
        };

        /// Returns true if pointer is not nullptr (mutable version)
        template<typename T>
        struct Valid<T*>
        {
          template<typename PTR>
          bool operator()(PTR _ptr) const
          {
            return _ptr != nullptr;
          }
        };

        /// Returns true if pointer is not nullptr (const version)
        template<typename T>
        struct Valid<T const*> : Valid<T*> {};

        /**@brief Returns true if all pointers in pointer set are not nullptr (mutable version)
          *@detail Returns false if set is empty
         **/
        template<typename T>
        struct Valid<std::set<T*>>
        {
          template<typename PTR_SET>
          bool operator()(const PTR_SET& _multiPtr) const
          {
            if (_multiPtr.empty()) return false;
            for (auto& _ptr : _multiPtr)
            {
              if (!_ptr) return false;
            }
            return true;
          }
        };

        /**@brief Returns true if all pointers in pointer set are not nullptr (const version)
          *@detail Returns false if set is empty
         **/
        template<typename T>
        struct Valid<std::set<T const*>> : Valid<std::set<T*>> {};
      }

      /// Function which determines if a pointer or a pointer set is valid
      template<typename T>
      bool valid(const T& _ptr)
      {
        return functor::Valid<T>()(_ptr);
      }
    }
  }
}
