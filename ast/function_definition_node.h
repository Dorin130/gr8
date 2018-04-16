#ifndef __GR8_FUNCTIONDEFINITIONNODE_H__
#define __GR8_FUNCTIONDEFINITIONNODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/basic_type.h>

namespace gr8 {

  /**
   * Class for describing funtion definitions
   */
  class function_definition_node: public cdk::basic_node {
    int _qualifier;
    basic_type *_type;
    std::string _name;
    cdk::sequence_node *_args;
    block_node *_body;

  public:
    function_definition_node(int lineno, int qualifier, basic_type *type, std::string *name,
    cdk::sequence_node *args, block_node *body) :
        basic_node(lineno), _qualifier(qualifier), _type(type), _name(*name),
        _args(args), _body(body) {
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

    inline cdk::sequence_node *args() {
      return _args;
    }

    inline block_node *body() {
      return _body;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_definition_node(this, level);
    }

    public:
      static const int QUALIFIER_NONE = 0;
      static const int QUALIFIER_PUBLIC = 1;
      static const int QUALIFIER_USE = 2;

  };

} // cdk

#endif
