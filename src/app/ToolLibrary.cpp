#include "widget/ToolLibrary.h"
#include <QStandardItemModel>

#include <tomo/tool/Library.hpp>

namespace tomo
{
  ToolLibrary::ToolLibrary(QWidget* _parent) :
    QWidget(_parent),
    library_(nullptr)
  {
    setupUi(this);
  }

  ToolLibrary::~ToolLibrary()
  {
  }  
  
  void ToolLibrary::updateItems()
  {
    libraryTree->generate(searchBar->text().toStdString());
  }

  void ToolLibrary::selectionChanged()
  {
    ToolLibraryItem const* _item = libraryTree->getSelected();
    if (!_item)
    {
      toolDescription->hide();
      toolPreview->hide();
      return;
    }

    bool _showPreview = false;
    switch (_item->itemType())
    {
    case ToolLibraryItem::SORTER:
    case ToolLibraryItem::INVALID:
      break;
    case ToolLibraryItem::TOOLSET:
      toolDescription->setText(getDescription(_item->id().toolset()));
      break;
    case ToolLibraryItem::TOOL:
    case ToolLibraryItem::ITEM:
    case ToolLibraryItem::DEVICE:
      _showPreview = true;
      toolDescription->setText(getDescription(_item->id().toolset(),_item->id().typeId()));
      toolPreview->setItem(_item);
      break;
    default: return;
    }

    toolPreview->setVisible(_showPreview);
    toolPreview->update();
  }
    
  QString ToolLibrary::getDescription(
      std::string const& _toolset, 
      std::string const& _typeId) const
  {
    if (!library_) 
    {
      toolDescription->hide();
      return QString();
    }

    // Handle toolset item
    if (_typeId.empty())
    {
      auto _descStr = library_->config().get_optional<std::string>(
        tbd::ConfigPath("description") / 
        tbd::ConfigPath(_toolset) /
        tbd::ConfigPath("description")); 
      if (!_descStr)
      {
        toolDescription->hide();
        return QString();
      }
      toolDescription->show();
      return QString().fromStdString(
          _toolset + std::string(":\n") +
          _descStr.get());
    }

    // Handle Item, Tool or Device
    auto _descStr = library_->config().get_optional<std::string>(
        tbd::ConfigPath("description") /
        tbd::ConfigPath(_toolset) / 
        tbd::ConfigPath(_typeId));
    if (!_descStr)
    {
      toolDescription->hide();
      return QString();
    }
    toolDescription->show();
    return QString().fromStdString(
        base::ElementId(_toolset,_typeId).str() + std::string(":\n") +
        _descStr.get());
    
  }

  void ToolLibrary::library(tool::Library* _library)
  {
    library_=_library;
    libraryTree->library(_library);
    toolPreview->library(_library);
    updateItems();
  }
}
