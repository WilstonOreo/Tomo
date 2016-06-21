#include "widget/ToolchainItem.h"

#include "widget/ToolchainEditor.h"
#include <tomo/base/Id.hpp>
#include <tomo/base/Element.hpp>
#include <tomo/tool/Library.hpp>

namespace tomo
{
  namespace widget
  {
    ToolchainItem::ToolchainItem(ToolchainEditor* _editor,
        QGraphicsItem* _parent) :
      QGraphicsItem(_parent),
      editor_(_editor),
      library_(_editor->library()),
      hovered_(false)
    {
      setFlag(ItemSendsGeometryChanges);
      setFlag(ItemIsSelectable,true);
      setAcceptHoverEvents(true);
      setCacheMode(DeviceCoordinateCache);
    }
      
    ToolchainItem::ToolchainItem(tool::Library const* _library,QGraphicsItem* _parent) :
      QGraphicsItem(_parent),
      editor_(nullptr),
      library_(_library),
      hovered_(false)
    {
      setFlag(ItemSendsGeometryChanges);
      setAcceptHoverEvents(true);
      setCacheMode(DeviceCoordinateCache);
    }

    ToolchainItem::~ToolchainItem() {}

    QColor ToolchainItem::lightColor() const
    {
      return QColor(QString().fromStdString(colorStr("light")));
    }

    QColor ToolchainItem::darkColor() const
    {
      return QColor(QString().fromStdString(colorStr("dark")));
    }

    QColor ToolchainItem::fontColor() const
    {
      return QColor(QString().fromStdString(colorStr("font")));
    }
    
    bool ToolchainItem::pathExists(std::string const& _path) const
    {
      if (!library()) return false;
      return library()->config().exists(ConfigPath("colorset") / ConfigPath(_path)); 
    }
      
    std::string ToolchainItem::colorStr(std::string const& _var) const
    {
      if (!library()) return std::string();

      if (!valid())
      {
        std::string _path(std::string("invalid_") + _var);
        if (!pathExists(_path))
        { 
          return (_var != "light") ? "#e74c3c" : "#6b2018"; 
        } else
        {
          return library()->config().get<std::string>(tbd::ConfigPath("colorset") / tbd::ConfigPath(_path));
        }
      }

      auto& _cfg = library()->config();
      tbd::ConfigPath _toolsetId(elementId().toolset().str());
      tbd::ConfigPath _typeId(elementId().typeId().str());
      tbd::ConfigPath _path = _toolsetId / _typeId / tbd::ConfigPath(_var);

      std::cout << _path.dump() << std::endl;
      if (pathExists(_path.dump()))
      {
        return _cfg.get<std::string>(ConfigPath("colorset") /_path);
      } 
      _path =  _toolsetId / tbd::ConfigPath(_var); 
      if (pathExists(_path.dump()))
      {
        return _cfg.get<std::string>(ConfigPath("colorset") /_path);
      }
      return (_var != "light") ? "#405959" : "#9ed8db"; 
    }
  }
}
