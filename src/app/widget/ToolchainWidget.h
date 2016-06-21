#include <QTreeView>
#include <QStandardItemModel>

#include "glwidget.h"
#include <tomo.hpp>
#include <tomo/query/Toolchain.hpp>

namespace tomo
{
  class ToolchainWidgetItem : public QStandardItem 
  {
  public:
    ToolchainWidgetItem(const std::string& _text,
        base::Element* _element);

    TBD_PROPERTY_RO(base::Element*,element)
  };

  class ToolchainWidget : public QTreeView
  {
    Q_OBJECT
  public:
    typedef QTreeView inherited_type;
    typedef tomo::Toolchain chain_type;
    typedef tomo::tool::Interface tool_type;
    typedef tomo::base::Element element_type;
    typedef ToolchainWidgetItem widget_item_type;
    typedef GLWidget viewer_type;
    typedef std::set<element_type const*> elementset_type;

    ToolchainWidget(QWidget* _parent = nullptr);
 
    bool loadConfig(const std::string& _filename);
    bool loadToolchain(const std::string& _filename);

    void writeQuery();
    void writeQuery(const std::string& _query, element_type const* _element = nullptr);

    elementset_type checkedItems();
    element_type* selection();

    TBD_PROPERTY_REF(tbd::Config,config)
    TBD_PROPERTY_REF_RO(Toolchain,toolchain)
    TBD_PROPERTY_REF_RO(std::unique_ptr<query::Toolchain>,inquirer)
    TBD_PROPERTY_REF_RO(std::string,toolchainFilename)
    TBD_PROPERTY_REF(std::string,queryFilename)

    TBD_PROPERTY(viewer_type*,viewer)
 
  private: 
    void generate(const tomo::tool::ChainInterface* _toolchain, widget_item_type* _root);
    void prepareModel();  

    void checkedItems(widget_item_type* _item);
    widget_item_type* root();

    template<class ITEM, class COLUMNS>
    void addItem(ITEM* _root, const COLUMNS& _columns)
    {
      QList<QStandardItem*> _row;
      for (auto& _item : _columns) 
      {
        TOMO_DBG(_item->text().toStdString())
        _item->setEditable(false);
        _row << _item;
      }
      _root->appendRow(_row);
    }

    std::unique_ptr<QStandardItemModel> model_;
  };
}

