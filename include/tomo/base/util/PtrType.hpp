#pragma once

#include <set>

namespace tomo
{
  namespace base
  {
    namespace util
    {
      /**@brief Retrieved const and mutable pointer types
        *@tparam T Data type
        *@tparam IS_MULTI Flag if defined type is for multiple pointers
       **/
      template<typename T, bool IS_MULTI = false>
      struct PtrType
      {
        typedef T* mutable_type;
        typedef T const* const_type;
      };

      /**@brief Retrieved const and mutable pointer types and return a pointer set
        *@tparam T Data type
       **/
      template<typename T>
      struct PtrType<T,true>
      {
        typedef std::set<typename PtrType<T,false>::mutable_type> mutable_type;
        typedef std::set<typename PtrType<T,false>::const_type> const_type;
      };

      template<typename T>
      typename PtrType<T,true>::const_type toConst(const typename PtrType<T,true>::mutable_type& _ptrs)
      {
        typename PtrType<T,true>::const_type _constPtrs;
for (auto& _ptr : _ptrs)
          _constPtrs.insert(_ptr);
        return _constPtrs;
      }
    }
  }
}
