#pragma once

#include <tomo/base/debug.hpp>
#include <tbd/classregister.h>
#include "Serialization.hpp"

#define TOMO_BASE_TYPES\
  typedef tomo::base::Id id_type;\
  typedef tomo::base::IdSet idset_type;\
  typedef std::string token_type;\
  typedef std::vector<token_type> tokenlist_type;\
  typedef tomo::tool::Chain parent_type;\


#include "Id.hpp"

namespace tomo
{
  namespace tool
  {
    /// Forward declaration
    class Chain;
  }


  namespace base
  {
    template<typename T>
    using Ptr = std::unique_ptr<T>;

    /**@brief An element can be stored in a toolchain
      *@detail Can either be a tool, a toolchain or an item
     **/
    class Element : public virtual tbd::SerializationInterface
    {
    public:
      TOMO_BASE_TYPES

      /// An element needs a parent
      Element(parent_type const* _parent) :
        parent_(_parent) {}
      virtual ~Element() {}

      /**@brief An Element must have a unique type id
        *@returns ElementId of object
       **/
      virtual ElementId elementId() const = 0;

      /// Get parent of element (const version)
      inline parent_type const* parent() const
      {
        return parent_;
      }

      /// Returns true if element has items (is true only for toolchains)
      inline virtual bool hasChildren() const
      {
        return false;
      }

      inline virtual IdSet classes() const
      {
        return IdSet();
      }
    private:
      parent_type const* parent_;
    };
  }
  using base::Element;
}

#define TOMO_ELEMENTID(toolset,id)\
      static ElementId typeId() \
      {\
        return ElementId(toolset,#id);\
      }\
      inline virtual tomo::ElementId elementId() const { return typeId(); }

#define TOMO_ELEMENT(toolset,name)\
  TOMO_BASE_TYPES\
  TOMO_ELEMENTID(toolset,name)
