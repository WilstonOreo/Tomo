#pragma once

#include "Interface.hpp"

namespace tomo
{
  namespace visual
  {
    struct Node
    {
    private:
      typedef base::Element const* element_ptr_type;
    public:
      typedef std::unique_ptr<Node> ptr_type;
      typedef visual::Interface interface_type;
      typedef std::unique_ptr<tool::Interface> tool_type;

      Node() : 
        visualizer_(nullptr), 
        active_(false) {}

      Node(element_ptr_type _element, bool _active = false) :
        visualizer_(nullptr), 
        active_(_active)
      {
        visualizerTool_ = tool::Registry::create(_element->getTypeId() + Toolset::separator() + "visual",_element->parent());
        visualizer_ = dynamic_cast<interface_type*>(visualizerTool_.get());
        if (!visualizer_) return;
        visualizer_->element(_element);
      }

      inline Node* add(element_ptr_type _element)
      {
        if (!base::hasVisualizer(_element)) return nullptr;
        children_.emplace_back(new Node(_element));
        return children_.back().get();
      }

      inline void preprocess()
      {
        //if (!active_) return;
        if (visualizer_) 
        {
          visualizer_->preprocess();
        }
        for (auto& _c : children_)
          _c->preprocess();
      }

      inline void process()
      {
        if (!active_) return;
        if (visualizer_) 
        {
          visualizer_->process();
        }
        for (auto& _c : children_)
          _c->process();
      }
      
      inline void resize(int _w, int _h)
      {
       // if (!active_) return;
        if (visualizer_) 
        {
          visualizer_->resize(_w,_h);
        }
        for (auto& _c : children_)
          _c->resize(_w,_h);
      }

      template<typename...ARGS>
      void signal(ARGS&&..._args)
      { 
        if (!active_) return;
        if (visualizer_) 
        {
          visualizer_->signal(_args...);
        }
        for (auto& _c : children_)
          _c->signal(_args...);
      }
      
      inline void clear()
      {
        return children_.clear();
      }

      TBD_PROPERTY_RO(interface_type*,visualizer)
      TBD_PROPERTY_REF(bool,active)
      TBD_PROPERTY_REF_RO(std::list<ptr_type>,children)
    private:
      tool_type visualizerTool_;
    };
  }
}
