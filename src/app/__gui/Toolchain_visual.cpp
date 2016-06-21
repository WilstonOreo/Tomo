#include <tomo/visual/Toolchain.hpp>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include "tomo/visual/gl/Base.hpp"


namespace tomo
{
  namespace visual
  { 
    //// visual Toolchain
    Toolchain::Toolchain(tomo::Toolchain const& _toolchain) :
      toolchain_(_toolchain)
    {
      buildTree();
    }

    void Toolchain::process()
    {
      Node::process();
    }

    void Toolchain::preprocess()
    {
      buildTree();
      TOMO_DBG(Node::children().size())
      Node::preprocess();
    }

    void Toolchain::resize(int _w, int _h)
    {
      Node::resize(_w,_h);
    }

    void Toolchain::buildTree()
    {
      Node::clear();
      Visualizer<tomo::Toolchain>::element(&toolchain_);
      buildTree(&toolchain_,this);
    }

    void Toolchain::buildTree(tool::ChainInterface const* _toolchain, Node* _node)
    {
      for (auto& _item : _toolchain->items())
        _node->add(_item.second.get());

      for (auto& _tool : _toolchain->tools())
      {
        auto _subnode = _node->add(_tool.get());
        if (!_tool->hasChildren() || !_subnode) continue;

        auto _chain = static_cast<tool::ChainInterface*>(_tool.get());
        buildTree(_chain,_subnode);
      }
    }

    Toolchain::interface_type* Toolchain::getVisualizer(element_ptr_type _element)
    {
      return getVisualizer(_element,this);
    }

    void Toolchain::setActive(const elementset_type& _elements)
    {
      setActive(_elements,this);
    }
    void Toolchain::setActive(const elementset_type& _elements, Node* _node)
    {
      if (!_node->visualizer())
        _node->active(_node == this);
      else
        _node->active(_elements.count(_node->visualizer()->element()) != 0 || _node == this);
      
      for (auto& _subnode : _node->children())
      {
        setActive(_elements,_subnode.get());
      }
    }

    Toolchain::interface_type* Toolchain::getVisualizer(element_ptr_type _element, Node* _node)
    {
      if (_node->visualizer())
      if (_element == _node->visualizer()->element()) return _node->visualizer();

      for (auto& _subnode : _node->children())
      {
        auto _visualizer = getVisualizer(_element,_subnode.get());
        if (_visualizer) return _visualizer;
      }
      return nullptr;
    }
  }
}

