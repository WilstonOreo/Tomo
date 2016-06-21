#pragma once

#include "Node.hpp"
#include "Tracker.hpp"
#include "Light.hpp"
#include "Camera.hpp"

namespace tomo
{
  namespace visual
  {
    class Toolchain :
      public Visualizer<tomo::Toolchain>,
      private Node,
      public tool::Registrar<Toolchain>
    {
      typedef gex::color::Color3<> Color3;
      typedef gex::color::Color4<> Color4;
      tomo::Toolchain const& toolchain_;

    public:
      typedef Visualizer<tomo::Toolchain> base_type;
      TOMO_VISUALIZER(Toolchain)
      
      typedef visual::Interface interface_type;
      typedef std::set<element_ptr_type> elementset_type;

      Toolchain(tomo::Toolchain const& _toolchain);

      void preprocess();
      void process();
      void resize(int _w, int _h);

      template<typename...ARGS>
      void signal(ARGS&&..._args)
      {
        Node::signal(_args...);
      }

      interface_type* getVisualizer(element_ptr_type _element);
      void setActive(const elementset_type& _elements);

    private:
      interface_type* getVisualizer(element_ptr_type _element, Node* _node);
      void setActive(const elementset_type& _elements, Node* _node);
      void buildTree();
      void buildTree(tool::ChainInterface const* _toolchain, Node* _node);
    };
  }
}

