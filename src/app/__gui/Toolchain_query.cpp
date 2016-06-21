#include <tomo/query/Toolchain.hpp>

namespace tomo
{
  namespace query 
  { 
    Toolchain::Toolchain(tomo::Toolchain const& _toolchain, const QuotedString& _output_filename) :
      toolchain_(_toolchain),
      output_filename_(_output_filename)
    {
    }

    Toolchain::interface_type* Toolchain::getInquirer(element_ptr_type _element)
    {
      return getInquirer(_element,this);
    }

    Toolchain::interface_type* Toolchain::getInquirer(element_ptr_type _element, Node* _node)
    {
      if (_element == _node->inquirer()->element()) return _node->inquirer();

      for (auto& _subnode : _node->children())
      {
        auto _inquirer = getInquirer(_element,_subnode.get());
        if (_inquirer) return _inquirer;
      }
      return nullptr;
    }

    void Toolchain::buildTree()
    {
      Node::clear();
      interface_type::element(&toolchain_);
      buildTree(&toolchain_,this);
    }

    void Toolchain::buildTree(tool::ChainInterface const* _toolchain, Node* _node)
    {
      for (auto& _item : _toolchain->items())
        _node->add(_item.first,_item.second.get());

      size_t _toolId = 0; 
      for (auto& _tool : _toolchain->tools())
      {
        std::stringstream ss;
        ss << _toolId << ":" << transformIdType(_tool->getTypeId());

        auto _subnode = _node->add(ss.str(),_tool.get());
        if (!_tool->hasChildren() || !_subnode) continue;

        auto _chain = static_cast<tool::ChainInterface*>(_tool.get());
        buildTree(_chain,_subnode);
        ++_toolId;
      }
    }

    String& Toolchain::output_filename()
    {
      return output_filename_;
    }

    void Toolchain::process()
    {
      buildTree();
      Node::preprocess();
      Node::process();
      aggregate("",this);

      base_type::data().merge(toolchain_.log(),tbd::ConfigPath("log"));

      if (!std::string(output_filename()).empty())
      {
        base_type::data().save(output_filename());
      }
    }
    
    void Toolchain::aggregate(const std::string& _path, Node const* _node)
    {
      auto _data = _node->data();
      auto _subpath = _path;
      if (!_subpath.empty()) _subpath += Toolset::separator();
      _subpath += _node->path();  

      TOMO_DBG(_subpath)

      if (_data)
      {
        base_type::data().merge(*_data,_subpath);
      }

      for (auto& _c : _node->children())
      {
        aggregate(_subpath,_c.get());
      }
    }
    
    Toolchain::id_type Toolchain::transformIdType(const id_type& _typeId)
    {
      id_type _s = _typeId;
      replace(_s.begin(), _s.end(), Toolset::separator()[0], '_');
      return _s;
    }
  }
}

