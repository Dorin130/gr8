#ifndef __GR8_VARDECLARATIONNODE_H__
#define __GR8_VARDECLARATIONNODE_H__

#include <string>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/identifier_node.h>

namespace gr8 {

  /**
   * Class for describing binary operators.
   */
  class var_declaration_node: public cdk::basic_node {
    int _qualifier;
    basic_type *_type;
    std::string _name;
    cdk::expression_node *_init;

  public:
    var_declaration_node(int lineno, int qualifier, basic_type *type, std::string *name, 
        cdk::expression_node *init) :
        basic_node(lineno), _qualifier(qualifier), _type(type), _name(*name), _init(init) {
    }

  public:
    inline bool noQualifier() {
      return _qualifier == QUALIFIER_NONE;
    }

    inline bool isPublic() {
      return _qualifier == QUALIFIER_PUBLIC;
    }

    inline bool isUse() {
      return _qualifier == QUALIFIER_USE;
    }
    
    inline basic_type *type() {
      return _type;
    }

    inline std::string name() {
      return _name;
    }

    inline cdk::expression_node *init() {
      return _init;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_var_declaration_node(this, level);
    }

    public:
      static const int QUALIFIER_NONE = 0;
      static const int QUALIFIER_PUBLIC = 1;
      static const int QUALIFIER_USE = 2;



  };

} // cdk

#endif
