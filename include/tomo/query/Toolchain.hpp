#pragma once

#include <tomo/tool/Chain.hpp>
#include "Node.hpp"

namespace tomo
{
  namespace query
  {
    struct Toolchain :
      public Inquirer<tomo::Toolchain>,
      private Node,
      public tool::Registrar<Toolchain> 
    {
      typedef query::Interface interface_type;
      typedef Inquirer<tomo::Toolchain> base_type;

      tomo::Toolchain const& toolchain_;
      
      TBD_PARAMETER_LIST
      (
        (String) output_filename,
        (bool) date_suffix
      )

    public:
      TOMO_INQUIRER(Toolchain)

      Toolchain(tomo::Toolchain const& _toolchain, const QuotedString& _output_filename = QuotedString("")); 
      interface_type* getInquirer(element_ptr_type _element);
 
      String& output_filename();
      void process();
    
    private:
      interface_type* getInquirer(element_ptr_type _element, Node* _node);
      void buildTree();
      void buildTree(tool::ChainInterface const* _toolchain, Node* _node);


      void aggregate(const std::string& _path, Node const* _node);

      /// Replaces "." with "_" in typeIds
      static id_type transformIdType(const id_type& _typeId);
    };
  }
}
