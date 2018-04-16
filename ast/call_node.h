#ifndef __GR8_CALLNODE_H__
#define __GR8_CALLNODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>

namespace gr8 {

  /**
   * Class for describing call instruction
   */
  class call_node: public cdk::expression_node {
    std::string _name;
    cdk::sequence_node *_args;

  public:
    inline call_node(int lineno, std::string *name, cdk::sequence_node *args) :
        cdk::expression_node(lineno), _name(*name),  _args(args){
    }

  public:
    inline std::string name() {
      return _name;
    }

    inline cdk::sequence_node *args() {
      return _args;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_call_node(this, level);
    }

  };

} // gr8

#endif
