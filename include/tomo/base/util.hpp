#pragma once

#include <set>
#include "debug.hpp"
#include "util/print.hpp"
#include "util/parse.hpp"
#include "util/valid.hpp"
#include "util/init.hpp"
#include "util/for_each.hpp"

namespace tomo
{
  namespace base
  {
    namespace util
    {
      namespace
      {
        /// Template for retrieving parent types of a tool (mutable version)
        template<typename TOOL, bool IS_CONST = false>
        struct ParentType
        {
          typedef typename TOOL::mutable_parent_type type;
          typedef std::vector<type> multi_type;
        };

        /// Template for retrieving parent types of a tool (const version)
        template<typename TOOL>
        struct ParentType<TOOL,true>
        {
          typedef typename TOOL::const_parent_type type;
          typedef std::vector<type> multi_type;
        };
      }

      /// Get depth (= number of parents) of a tool
      template<typename TOOL>
      size_t depth(const TOOL& _tool)
      {
        size_t _depth = 0;
        typedef ParentType<TOOL,true> parent_type;
        for_each_parent(_tool,[&](const typename parent_type::type& _parent)
        {
          ++_depth;
        });
        return _depth;
      }

      /// Get all parents of a tool (mutable version)
      template<typename TOOL>
      typename ParentType<TOOL,false>::multi_type parents(TOOL& _tool)
      {
        typedef ParentType<TOOL,false> parent_type;
        typename parent_type::multi_type _parents;
        for_each_parent(_tool,[&](const typename parent_type::type& _parent)
        {
          _parents.push_back(_parent);
        });
        return _parents;
      }

      /// Get all parents of a tool (const version)
      template<typename TOOL>
      typename ParentType<TOOL,true>::multi_type parents(const TOOL& _tool)
      {
        typedef ParentType<TOOL,true> parent_type;
        typename parent_type::multi_type _parents;
        for_each_parent(_tool,[&](const typename parent_type::type& _parent)
        {
          _parents.push_back(_parent);
        });
        return _parents;
      }

    }
  }
}
