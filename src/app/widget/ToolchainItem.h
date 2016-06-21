#pragma once


#include <tbd/property.h>
#include <list>
#include <set>
#include <QGraphicsItem>

namespace tomo
{
  class ToolchainEditor;

  namespace base
  {
    struct ElementId;
    class Element;
  }

  namespace tool
  {
    struct Library;
  }

  namespace widget
  {
    class ToolchainItem : public QGraphicsItem
    {
    public:
      typedef std::set<base::Element const*> elements_type;
      ToolchainItem(ToolchainEditor*,QGraphicsItem* _parent);
      ToolchainItem(tool::Library const*,QGraphicsItem* _parent);
      ~ToolchainItem();

      QColor fontColor() const;
      QColor lightColor() const;
      QColor darkColor() const;
     
      virtual elements_type elements(bool _recursive = false) const = 0;
      virtual base::ElementId elementId() const = 0;
      virtual bool valid() const = 0;

      TBD_PROPERTY_RO(ToolchainEditor*,editor)
      TBD_PROPERTY(tool::Library const*,library)
      TBD_PROPERTY(bool,hovered)
    protected:
      std::string colorStr(std::string const& _var) const;
      bool pathExists(std::string const& _var) const;  
    };
  }
}
