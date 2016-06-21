#pragma once

#include "Interface.hpp"

namespace tomo
{
  namespace query
  {
    struct Node
    {
    private:
      typedef base::Element const* element_ptr_type;
    public:
      typedef std::unique_ptr<Node> ptr_type;
      typedef Interface interface_type;
      typedef std::unique_ptr<tool::Interface> tool_type;
      typedef Interface::data_type data_type;

      Node() : inquirer_(nullptr) {}
      Node(const std::string& _path, element_ptr_type _element) :
        path_(_path)
      {
        
        inquirerTool_ = tool::Registry::create(_element->getTypeId() + Toolset::separator() + "query",_element->parent());
        inquirer_ = dynamic_cast<interface_type*>(inquirerTool_.get());
        if (!inquirer_) return;
        inquirer_->element(_element);
      }

      inline Node* add(const std::string& _path, element_ptr_type _element)
      {
        if (!base::hasInquirer(_element)) return nullptr;
        children_.emplace_back(new Node(_path,_element));
        return children_.back().get();
      }

      inline data_type const* data() const
      {
        if (!inquirer_) return nullptr;
        return &inquirer()->data();        
      }

      inline void preprocess()
      {
        if (inquirer()) inquirer_->preprocess();
        for (auto& _c : children_)
          _c->preprocess();
      }

      inline void process()
      {
        if (inquirer()) inquirer_->process();
        for (auto& _c : children_)
          _c->process();
      }

      inline void clear()
      {
        return children_.clear();
      }

      TBD_PROPERTY_RO(interface_type*,inquirer)
      TBD_PROPERTY_REF_RO(std::string,path)
      TBD_PROPERTY_REF_RO(std::list<ptr_type>,children)
    private:
      tool_type inquirerTool_;
    };
  }
}
